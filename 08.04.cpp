#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <algorithm>

static const std::string TARGET = "methinksitislikeaweasel";
static const int COPIES = 100;
static const double MUTATION_RATE = 0.05;
static const int LEN = 23;

int fitness(const std::string &s)
{
    int diff = 0;
    for (int i = 0; i < LEN; ++i)
    {
        if (s[i] != TARGET[i])
        {
            ++diff;
        }
    }
    return diff;
}

int main()
{
    std::random_device rd;
    std::default_random_engine engine(rd());
    std::uniform_int_distribution<int> letter_dist(0, 25);
    std::uniform_real_distribution<double> mut_dist(0.0, 1.0);

    std::string parent(LEN, ' ');
    for (int i = 0; i < LEN; ++i)
    {
        parent[i] = 'a' + letter_dist(engine);
    }

    for (int gen = 0;; ++gen)
    {
        int score = fitness(parent);
        std::cout << "Generation " << gen << ": " << parent
                  << "  (distance = " << score << ")\n";
        if (score == 0)
        {
            break;
        }

        std::vector<std::string> offspring(COPIES);
        for (int c = 0; c < COPIES; ++c)
        {
            offspring[c] = parent;
            for (int i = 0; i < LEN; ++i)
            {
                if (mut_dist(engine) < MUTATION_RATE)
                {
                    offspring[c][i] = 'a' + letter_dist(engine);
                }
            }
        }

        parent = *std::min_element(offspring.begin(), offspring.end(),
                                   [](const std::string &a, const std::string &b)
                                   {
                                       return fitness(a) < fitness(b);
                                   });
    }
}

/*
Generation 0: pzyupmkntzjbxptfuixxgio  (distance = 22)
Generation 1: oztupmkntzjbxptfuixxgio  (distance = 21)
Generation 2: oztupmkntzjbxitfuixxgio  (distance = 20)
Generation 3: oztopnkntzjbxitfuixxgio  (distance = 19)
Generation 4: mrtopnkntzjbxitfuixxgio  (distance = 18)
Generation 5: mrtopnkstzjbxitfuixxgio  (distance = 17)
Generation 6: mrtopnkstzrbxitfuixagix  (distance = 16)
Generation 7: mrtopnkstzrbxitfaixagix  (distance = 15)
Generation 8: mrtopnkstzrbxitfaixagex  (distance = 14)
Generation 9: mrtopnkstzrbxiueaixagex  (distance = 13)
Generation 10: mrtopnkstzrbxiueawxakex  (distance = 12)
Generation 11: mrtoinkstzrbxiueawxakex  (distance = 11)
Generation 12: mrtoinkstzrsxiueawxakex  (distance = 10)
Generation 13: mrtoinkstzosxiueawxakex  (distance = 10)
Generation 14: mrtoinkstzosxiueawxakel  (distance = 9)
Generation 15: mrtoinkstzosxiueawxakel  (distance = 9)
Generation 16: mrtoinkstzosxikeawxakel  (distance = 8)
Generation 17: mrtoinksttzsxikeawxakel  (distance = 7)
Generation 18: mrthinksttzsxikeawxakel  (distance = 6)
Generation 19: mrthinksitzsxikeawxakel  (distance = 5)
Generation 20: mrthinksitzsxikeawxakel  (distance = 5)
Generation 21: mrthinksitzsxikeawxasel  (distance = 4)
Generation 22: mrthinksitzsxikeawxasel  (distance = 4)
Generation 23: mrthinksitzsxikeawxasel  (distance = 4)
Generation 24: mrthinksitzsxikeawxasel  (distance = 4)
Generation 25: mrthinksitzsxikeawxasel  (distance = 4)
Generation 26: mrthinksitzsxikeaweasel  (distance = 3)
Generation 27: mrthinksitzsxikeaweasel  (distance = 3)
Generation 28: mrthinksitzsxikeaweasel  (distance = 3)
Generation 29: mrthinksitzsxikeaweasel  (distance = 3)
Generation 30: methinksitzsxikeaweasel  (distance = 2)
Generation 31: methinksitzslikeaweasel  (distance = 1)
Generation 32: methinksitzslikeaweasel  (distance = 1)
Generation 33: methinksitoslikeaweasel  (distance = 1)
Generation 34: methinksitoslikeaweasel  (distance = 1)
Generation 35: methinksitoslikeaweasel  (distance = 1)
Generation 36: methinksitoslikeaweasel  (distance = 1)
Generation 37: methinksitoslikeaweasel  (distance = 1)
Generation 38: methinksitislikeaweasel  (distance = 0)
*/