#pragma once
#include<vector>

namespace L_Systems
{
    using alphabet_T = int;

    class Rules
    {
    public:
        Rules(alphabet_T alphabet_size) : alphabet_size(alphabet_size)
        {
            rules.resize(alphabet_size);
        }

        void add(alphabet_T from, std::vector<alphabet_T> to)
        {
            rules[from] = to;
        }
        std::vector<alphabet_T>* operator[](alphabet_T key)
        {
            return &rules[key];
        }
    private:
        alphabet_T alphabet_size;
        std::vector<std::vector<alphabet_T>> rules;
    };

    class L_System
    {
    public:
        L_System(Rules rule_set, std::vector<alphabet_T> starting_word) : rule_set(rule_set), starting_word(starting_word)
        {
            reset();
        }

        void run(int steps)
        {
            for (int i = 0; i < steps; i++)
            {
                step();
            }
        }

        void reset()
        {
            word = std::vector<alphabet_T>(starting_word);
        }

        std::vector<alphabet_T> word{};

    private:
        Rules rule_set;
        std::vector<alphabet_T> starting_word;

        void step()
        {
            std::vector<alphabet_T> new_word{};

            for (alphabet_T letter : word)
            {
                std::vector<alphabet_T>* expansion{ rule_set[letter] };
                for (alphabet_T new_letter : *expansion) new_word.push_back(new_letter);
            }

            word.swap(new_word);
        }
    };
}