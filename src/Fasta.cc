#include "Fasta.hh"

#include <cstring>
#include <regex>

utils::Fasta::Fasta(std::istream &is)
{
    _read(is);
}

std::vector<std::string> utils::Fasta::split(std::string str, char del){

    std::string temp = "";
    std::vector<std::string> result;

    for(int i=0; i<(int)str.size(); i++){
        if(str[i] != del){
            temp += str[i];
        }
        else{
            result.push_back(temp);
            temp = "";
        }
    }
    result.push_back(temp);
    return result;
}

void utils::Fasta::write_to(std::ostream &os, bool with_identification) const
{
    if (with_identification)
        write_to(os, sequences.cbegin(), sequences.cend(), identifications.cbegin());
    else
        write_to(os, sequences.cbegin(), sequences.cend());
}

void utils::Fasta::_read(std::istream &is)
{
    std::string each_line;
    std::string each_sequence;
    
    for (bool flag = false; std::getline(is, each_line); )
    {
        each_line.erase(std::remove(each_line.begin(), each_line.end(), '\r'), each_line.end());
        
        if (each_line.size() == 0)
            continue;

        if (each_line[0] == '>')
        {
            std::string tmp = each_line.substr(1);
            if(tmp.find(' ')!=tmp.npos)
            {
                identifications.push_back(utils::Fasta::split(each_line.substr(1), ' ')[0]);
            }
            else if(tmp.find(',')!=tmp.npos)
            {
                std::regex regexp(",");
                identifications.push_back(std::regex_replace(each_line.substr(1), regexp, "_"));
            }
            else
            {
                identifications.push_back(each_line.substr(1));
            }
            if (flag)
                sequences.push_back(std::move(each_sequence));
            flag = true;
        }
        else if (flag)
        {
            std::transform(each_line.begin(),each_line.end(),each_line.begin(),toupper);
            each_sequence += each_line;
        }
    }
    std::transform(each_sequence.begin(),each_sequence.end(),each_sequence.begin(),toupper);
    sequences.push_back(each_sequence);
}

void utils::Fasta::cut_and_write(std::ostream &os, const std::string &sequence)
{
    const size_t sequence_length = sequence.size();

    char *cut_sequence = new char[sequence_length + sequence_length / max_line_length + 1];
    size_t des_index = 0;
    for (size_t src_index = 0; src_index < sequence_length; src_index += max_line_length)
    {
        if (src_index) cut_sequence[des_index++] = '\n';

        size_t write_length = sequence_length - src_index;
        if (write_length > max_line_length) write_length = max_line_length;

        memcpy(cut_sequence + des_index, sequence.data() + src_index, write_length);
        des_index += write_length;
    }
    cut_sequence[des_index] = 0;

    os << cut_sequence;
    delete[] cut_sequence;
}
