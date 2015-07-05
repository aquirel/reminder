#include <cstdio>
#include <cstring>
#include <ctime>
#include <cctype>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

struct Person
{
    tm birthday;
    string name;
};

int main(int argc, char *argv[])
{
    const char *rc_file_name = ".reminderrc";
    string rc_file_path;

    if (2 <= argc)
    {
        if (0 == strcmp("--help", argv[1]) ||
            0 == strcmp("-h", argv[1]))
        {
            printf("Usage: %s [.reminderrc]\n", argv[0]);
            return 0;
        }
        else
        {
            rc_file_path = argv[1];
        }
    }

    if (0 == rc_file_path.length())
    {
        rc_file_path = getenv("HOME") ?: ".";

        if ('/' != rc_file_path.back())
        {
            rc_file_path.append("/");
        }

        rc_file_path.append(rc_file_name);
    }

    vector<Person> persons;
    unsigned remind_before = 3; // In days.

    ifstream rc_file(rc_file_path);

    string remind_before_prefix = "remind_before",
           buf;

    while (rc_file.good())
    {
        getline(rc_file, buf, '\n');

        if (0 == buf.length() ||
            '#' == buf[0] ||
            ';' == buf[0])
        {
            continue;
        }

        if (equal(remind_before_prefix.begin(), remind_before_prefix.end(), buf.begin()))
        {
            buf = buf.substr(remind_before_prefix.length());
            while (0 < buf.length() && isspace(buf[0]))
            {
                buf.erase(0, 1);
            }

            if (0 < buf.length() && '=' == buf[0])
            {
                buf.erase(0, 1);

                if (0 < buf.length())
                {
                    remind_before = stoul(buf);
                }
            }
        }
        else
        {
            unsigned year, month, day;
            if (3 != sscanf(buf.c_str(), "%4u-%02u-%02u", &year, &month, &day))
            {
                continue;
            }

            tm birthday;
            memset(&birthday, 0, sizeof(birthday));

            birthday.tm_year = year - 1900;
            birthday.tm_mon = month - 1;
            birthday.tm_mday = day;

            if (-1 == mktime(&birthday))
            {
                continue;
            }

            auto delimiter = find_if(buf.begin(), buf.end(), [] (const char c) { return isspace(c); });
            if (buf.end() == delimiter)
            {
                continue;
            }

            buf = buf.substr(delimiter - buf.begin());

            while (0 < buf.length() && isspace(buf[0]))
            {
                buf.erase(0, 1);
            }

            while (0 < buf.length() && isspace(buf.back()))
            {
                buf.erase(buf.length() - 1, 1);
            }

            Person new_person {
                .birthday = birthday,
                .name = buf
            };

            persons.push_back(new_person);
        }
    }

    rc_file.close();

    time_t time_since_epoch = time(nullptr);
    tm now = *localtime(&time_since_epoch);

    for (unsigned shift = 0; shift <= remind_before; ++shift)
    {
        tm birthday = now;
        birthday.tm_mday += shift;
        mktime(&birthday);

        for (auto person = persons.begin(); person != persons.end(); ++person)
        {
            if (birthday.tm_mon == person->birthday.tm_mon &&
                birthday.tm_mday == person->birthday.tm_mday)
            {
                char buf[16];
                strftime(buf, sizeof(buf), "%F", &birthday);

                printf("%s has birthday on %s ", person->name.c_str(), buf);

                if (0 == shift)
                {
                    printf("(TODAY!). ");
                }
                else
                {
                    printf("(in %u days). ", shift);
                }

                printf("He/she becomes %u.\n", birthday.tm_year - person->birthday.tm_year);
            }
        }
    }

    return 0;
}
