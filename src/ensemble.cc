#include <chrono> // std::chrono 
#include <ctime> // std::time_t / std::localtime 

#include <vector>
#include <numeric>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <regex>
#include <unistd.h>
#include <filesystem>
#include "Fasta.hh"


#if defined(_WIN32)
#include <windows.h>
#include <psapi.h>
#include <process.h>
#include <io.h>
#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
#include <sys/types.h>
#include <dirent.h>
// #include <malloc.h>
#include <unistd.h>
#include <sys/resource.h>
#include <pthread.h>
#if defined(__APPLE__) && defined(__MACH__)
#include <mach/mach.h>
#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
#include <fcntl.h>
#include <procfs.h>
#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
#include <stdio.h>
#endif
#else
#error "Cannot define getPeakRSS( ) or getCurrentRSS( ) for an unknown OS."
#endif

size_t getPeakRSS()
{
#if defined(_WIN32)
    /* Windows -------------------------------------------------- */
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
    return (size_t)info.PeakWorkingSetSize;

#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
    /* AIX and Solaris ------------------------------------------ */
    struct psinfo psinfo;
    int fd = -1;
    if ((fd = open("/proc/self/psinfo", O_RDONLY)) == -1)
        return (size_t)0L;        /* Can't open? */
    if (read(fd, &psinfo, sizeof(psinfo)) != sizeof(psinfo))
    {
        close(fd);
        return (size_t)0L;        /* Can't read? */
    }
    close(fd);
    return (size_t)(psinfo.pr_rssize * 1024L);
#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
    /* BSD, Linux, and OSX -------------------------------------- */
    struct rusage rusage;
    getrusage(RUSAGE_SELF, &rusage);
#if defined(__APPLE__) && defined(__MACH__)
    return (size_t)rusage.ru_maxrss;
#else
    return (size_t)(rusage.ru_maxrss * 1024L);
#endif
#else
    /* Unknown OS ----------------------------------------------- */
    return (size_t)0L;            /* Unsupported. */
#endif
}

void cout_cur_time()
{
    auto now = std::chrono::system_clock::now();
    // Get milliseconds
    uint64_t dis_millseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count()
        - std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() * 1000;
    time_t tt = std::chrono::system_clock::to_time_t(now);
    auto time_tm = localtime(&tt);
    char strTime[25] = { 0 };
    sprintf(strTime, "%d-%02d-%02d %02d:%02d:%02d | ", time_tm->tm_year + 1900,
        time_tm->tm_mon + 1, time_tm->tm_mday, time_tm->tm_hour,
        time_tm->tm_min, time_tm->tm_sec);
    std::cout << strTime;
}

// Preset evaluation parameters 
static constexpr long long     MISMATCH = -1;
static constexpr long long        MATCH =  1;
static constexpr long long GAPEXTENSION = -2;
static constexpr long long      GAPOPEN =  0;

std::string raw_data;
std::vector<std::string> child_msa;
std::string output;
std::ostream *v1logger;
std::ofstream f_matches;

static unsigned map[128];
// Dilimit schedule
class Pace
{
public:
    explicit Pace(std::vector<std::string> const &sequences)
        : _row(sequences.size())
        , _col(sequences.front().size())
        , _j(0)
        , _pace(_row)
        , _sequences(sequences)
    {
    }

    Pace &operator++() noexcept
    {
        for (unsigned i = 0; i != _row; ++i)
            _pace[i] += map[_sequences[i][_j]];
        ++_j;
        return *this;
    }

    // assumes that rhs._row == _row
    bool operator<(Pace const &rhs) const noexcept
    {
        bool flag = false;
        for (unsigned i = 0; i != _row; ++i)
        {
            if (_pace[i] > rhs[i]) return false;
            if (_pace[i] < rhs[i]) flag = true;
        }
        return flag;
    }

    // assumes that rhs._row == _row
    bool operator<=(Pace const &rhs) const noexcept
    {
        for (unsigned i = 0; i != _row; ++i)
            if (_pace[i] > rhs[i])
                return false;
        return true;
    }

    // assumes that rhs._row == _row
    bool operator==(Pace const &rhs) const noexcept
    {
        return memcmp(_pace.data(), rhs._pace.data(), sizeof(unsigned) * _row) == 0;
    }

    unsigned operator[](unsigned i) const noexcept
    {
        return _pace[i];
    }

    unsigned where() const noexcept
    {
        return _j;
    }

    unsigned finished() const noexcept
    {
        return _j == _col;
    }

    unsigned unfinished() const noexcept
    {
        return _j != _col;
    }

private:
    unsigned const _row, _col;
    unsigned _j;
    std::vector<unsigned> _pace;
    std::vector<std::string> const &_sequences;
};
// Dilimit profile row and column
// row: the number of sequences
// lhs_col: the column number of profile1
// rhs_col: the column number of profile2 
static unsigned row, lhs_col, rhs_col;

static void preprocess()
{
    std::fill(map, map + 128, 1);
    map['-'] = 0;
}

inline void display_help(const char* prog) {
	printf("TPMA v1.0, by Yixiao Zhai, June 2023.\n");
	printf("Usage: %s -a <initial_alignments_list> -r <raw_data> -o <output> \n", prog);
	printf("\t -a is used to specify the paths of all initial alignments to be merged\n");
	printf("\t -r is used to specify the path of raw data, a file in FASTA format\n");
	printf("\t -o is used to specify the output for TPMA result, a file in FASTA format\n");
	printf("\t -h print help message\n");

	printf("Example:\n\t");
	printf("./tpma -a PATH/test_msa1.fa PATH/test_msa2.fa -r test.fa -o test_tpma.fa\n\n");
}

inline void get_pars(int argc, char* argv[]) {
	v1logger = &std::cout;
	bool is1 = false, is2 = false, is3 = false;
	int oc;
	while ((oc = getopt(argc, argv, "r:a:o:h")) >= 0) {
		switch (oc) {
			case 'r':
				raw_data = optarg;
				is1 = true;
				break;
			case 'a':
                #ifdef __APPLE__
                // macOS specific code
                child_msa.push_back(optarg);
                while (optind < argc && argv[optind][0] != '-') {
                    child_msa.push_back(argv[optind]);
                    optind++;
                }
                #elif defined(__linux__)
                // Linux specific code
                    for (int i = optind - 1; i < argc && *argv[i] != '-'; ++i) {
                        child_msa.push_back(argv[i]);
                    }
                #endif
				is2 = true;
				break;
			case 'o':
				output = optarg;
				is3 = true;
				break;
			case 'h':
				display_help(argv[0]);
				exit(0);
			case '?':
				std::cerr << "ERROR: Error parameters.\n Please run './tpma -h'\n";
				exit(1);
				break;
		}
	}

	if (!is1) {
		fprintf(stderr, "ERROR: Required parameter -r is not provided!!\n");
		exit(1);
	}

    if (!is2) {
		fprintf(stderr, "ERROR: Required parameter -a is not provided!!\n");
		exit(1);
	}

    if (!is3) {
		fprintf(stderr, "ERROR: Required parameter -o is not provided!!\n");
		exit(1);
	}
	
	std::ifstream f;

	f.open(raw_data);
	if (f.fail()) {
		fprintf(stderr, "ERROR: Raw data file '%s' does not exist.\n", raw_data.c_str());
		exit(1);
	}
	f.close();

	f_matches.open(output);
	if (f_matches.fail()) {
		fprintf(stderr, "ERROR: The output file '%s' can not be created.\n", output.c_str());
		exit(1);
	}
	f_matches.close();
}

static void order(utils::Fasta &lhs, utils::Fasta const &rhs)
{
    std::vector<std::string> new_sequences;
    for(int i = 0; i < rhs.identifications.size(); i++)
    {
        auto identifications = std::find(lhs.identifications.begin(),
                                        lhs.identifications.end(),
                                        rhs.identifications[i]);
        if(identifications != lhs.identifications.end())
        {
            int index = std::distance(lhs.identifications.begin(), identifications);
            new_sequences.push_back(lhs.sequences[index]);
        }
    }
    lhs.sequences.assign(new_sequences.begin(), new_sequences.end());
    lhs.identifications.assign(rhs.identifications.begin(), rhs.identifications.end());
    new_sequences.clear();
}

static std::vector<unsigned> lhs_cut, rhs_cut;

static void cut(utils::Fasta const &lhs, utils::Fasta const &rhs)
{
    lhs_cut.push_back(0);
    rhs_cut.push_back(0);

    // the initial state of the loop below guarantees that rhs_pace < lhs_pace returns false
    for (Pace lhs_pace(lhs.sequences), rhs_pace(rhs.sequences); lhs_pace.unfinished() || rhs_pace.unfinished(); )
    {
        // assumes that rhs_pace < lhs_pace returns false
        for (; lhs_pace.unfinished() && (rhs_pace < lhs_pace) == false; )
            if (++lhs_pace == rhs_pace)
            {
                lhs_cut.push_back(lhs_pace.where());
                rhs_cut.push_back(rhs_pace.where());
            }
        // after the loop above, lhs_pace < rhs_pace returns false
        for (; rhs_pace.unfinished() && (lhs_pace < rhs_pace) == false; )
            if (lhs_pace == ++rhs_pace)
            {
                lhs_cut.push_back(lhs_pace.where());
                rhs_cut.push_back(rhs_pace.where());
            }
        // after the loop above, rhs_pace < lhs_pace returns false
    }
}

static long long score(utils::Fasta const &fasta, unsigned j)
{
    unsigned counts[128];
    memset(counts, 0, sizeof(counts));
    for (unsigned i = 0; i != row; ++i)
        ++counts[fasta.sequences[i][j]];
    unsigned const a = counts['a'] + counts['A'];
    unsigned const g = counts['g'] + counts['G'];
    unsigned const c = counts['c'] + counts['C'];
    unsigned const t = counts['t'] + counts['T'] + counts['u'] + counts['U'];
    unsigned const gap = counts['-'];
    unsigned const n = std::accumulate(counts, counts + 128, 0u) - a - g - c - t - gap;
    return ((a + c) * (g + t) + a * c + g * t) * MISMATCH + (a * (a - 1) + c * (c - 1) + g * (g - 1) + t * (t - 1)) / 2 * MATCH + (a + c + g + t + n) * gap * GAPEXTENSION;
}

static long long score(utils::Fasta const &fasta, unsigned l, unsigned r)
{
    long long s = 0;
    for (unsigned i = l; i != r; ++i)
        s += score(fasta, i);
    return s;
}

static std::vector<std::string> result;

static void append(utils::Fasta const &fasta, unsigned l, unsigned r)
{
    for (unsigned i = 0; i != row; ++i)
        result[i].insert(result[i].cend(), fasta.sequences[i].cbegin() + l, fasta.sequences[i].cbegin() + r);
}

static void merge(utils::Fasta const &lhs, utils::Fasta const &rhs)
{
    result.resize(row);
    for (std::string &sequence : result)
        sequence.reserve(std::max(lhs_col, rhs_col));
    
    unsigned const n = lhs_cut.size();

    for (unsigned i = 1; i != n; ++i)
    {
        if (score(lhs, lhs_cut[i - 1], lhs_cut[i]) < score(rhs, rhs_cut[i - 1], rhs_cut[i]))
            append(rhs, rhs_cut[i - 1], rhs_cut[i]);
        else
            append(lhs, lhs_cut[i - 1], lhs_cut[i]);
    }
    lhs_cut.clear();
    rhs_cut.clear();
}

static utils::Fasta read_from(std::string file_path)
{
    std::ifstream file(file_path);
    if (!file) exit(0);
    utils::Fasta fasta(file);
    file.close();
    return fasta;
}

static void merge_core(std::vector<utils::Fasta> fasta_set, std::string result_name)
{
    utils::Fasta       lhs = fasta_set[0];
    utils::Fasta const rhs = fasta_set[1];
    
    row = lhs.sequences.size();
    lhs_col = lhs.sequences.front().size();
    rhs_col = rhs.sequences.front().size();
    
    preprocess();
    cut(lhs, rhs);
    merge(lhs, rhs);
    
    std::ofstream ofs(result_name);
    if (!ofs) exit(0);
    lhs.sequences = std::move(result);
    lhs.write_to(ofs);
    ofs.close();

    unsigned index = 2;
    while(index < fasta_set.size())
    {
        utils::Fasta       lhs = fasta_set[index];
        utils::Fasta const rhs = read_from(result_name);
        
        row = lhs.sequences.size();
        lhs_col = lhs.sequences.front().size();
        rhs_col = rhs.sequences.front().size();
        
        preprocess();
        cut(lhs, rhs);
        merge(lhs, rhs);
        
        std::ofstream ofs(result_name);
        if (!ofs) exit(0);
        lhs.sequences = std::move(result);
        lhs.write_to(ofs);
        ofs.close();

        index++;
    }
}

static void refine(std::string raw_data_path, std::vector<std::string> alignments_list, std::string result_name)
{   
    
    clock_t startTime, endTime, merge_endTime;
    startTime = clock();

    std::vector<utils::Fasta> fasta_set;
    utils::Fasta raw_data = read_from(raw_data_path);

    cout_cur_time();
    std::cout <<  "Finish loading raw dataset." << std::endl;
    
    cout_cur_time();
    std::cout <<  "Start checking initial alignment." << std::endl;

    int flag = 0;
    int processed = 0;
    std::regex regexp("[^a-zA-z]");

    // Store fasta and sp score
    std::vector<std::pair<utils::Fasta,long long>> sp_scores;
    
    // Check data begin
    for (int each_line=0; each_line < alignments_list.size(); each_line++)
    {
        // Whether the initial alignment exists
        std::ifstream f;
        f.open(alignments_list[each_line]);
        if (f.fail()) {
            fprintf(stderr, "ERROR: The initial alignments '%s' does not exist.\n", alignments_list[each_line].c_str());
            exit(1);
        }
        f.close();

        flag = 0;
        // initial alignment
        utils::Fasta fasta = read_from(alignments_list[each_line]);

        std::vector<std::string> new_sequences;

        for(int i = 0; i < raw_data.identifications.size(); i++)
        {
            if(processed==0)
            {
                raw_data.sequences[i] = std::regex_replace(raw_data.sequences[i], regexp, "");
            }
            auto identifications = std::find(fasta.identifications.begin(),
                                            fasta.identifications.end(),
                                            raw_data.identifications[i]);
            
            if(identifications != fasta.identifications.end())
            {
                int index = std::distance(fasta.identifications.begin(), identifications);
                std::string tmp = fasta.sequences[index];
                if(raw_data.sequences[i] != std::regex_replace(tmp, regexp, ""))
                {
                    std::cout << "Warning: " << each_line << " can't been processed." << std::endl;
                    flag = 1;
                    break;
                }
                else
                {
                    new_sequences.push_back(fasta.sequences[index]);
                }
            }
            else
            {
                fprintf(stderr, "ERROR: The initial alignments '%s' is different from the raw dataset.\n", alignments_list[each_line].c_str());
                exit(1); 
            }
        }
        fasta.sequences.assign(new_sequences.begin(), new_sequences.end());
        fasta.identifications.assign(raw_data.identifications.begin(), raw_data.identifications.end());
        new_sequences.clear();

        if(flag == 0)
        {
            long long sp_score = score(fasta, 0, fasta.sequences[0].size());
            sp_scores.push_back(std::make_pair(fasta, sp_score));
        }
        processed += 1;
    }

    // Order the files based on sp score
    std::sort(sp_scores.begin(), sp_scores.end(), [](const auto& score1, const auto& score2) { return score1.second > score2.second; });
    for (const auto& score : sp_scores){
        fasta_set.push_back(score.first);
    }

    endTime = clock();
    cout_cur_time();
    std::cout << "Finish checking all initial alignments." << std::endl;
    // std::cout << "# Checking time: " <<(double)(endTime - startTime)/CLOCKS_PER_SEC << " s." << std::endl;
    
    cout_cur_time();
    std::cout << "Start combining initial alignments." << std::endl;

    merge_core(fasta_set, result_name);
    
    cout_cur_time();
    std::cout << "Finish combining initial alignments." << std::endl; 
    // merge_endTime = clock();
    // std::cout << "# Ensemble time  : " <<(double)(merge_endTime - endTime)/CLOCKS_PER_SEC << " s." << std::endl;
    
}

int main(int argc, char **argv)
{
    if(argc < 3)
    {
        display_help(argv[0]);
		return 1;
    }
    // Save argv
    std::vector<std::string> argv_copy;
    for (int i = 0; i < argc; i++) {
        argv_copy.push_back(argv[i]);
    }

    get_pars(argc, argv);

    clock_t startTime, endTime;
    startTime = clock();

    refine(raw_data, child_msa, output);
    
    endTime = clock();

    cout_cur_time();
    std::cout << "Done." << std::endl;

    std::cout << "# Command line: ";
    for (int i = 0; i < argc; ++i) {
        std::cout << argv_copy[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "# Total time  : " <<(double)(endTime - startTime)/CLOCKS_PER_SEC << " s." << std::endl;
    std::cout << "# Memory usage: " << (double)getPeakRSS() / (1024 * 1024) << " MB." << std::endl;
    std::cout << "# TPMA is available from https://github.com/malabz/TPMA" << std::endl;
    return 0;
}
