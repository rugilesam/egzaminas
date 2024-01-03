#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <regex>

    // Funkcija, kuri pašalina skyrybos ženklus iš žodžio galo
    std::string removePunctuation(const std::string& word) {
        std::string result = word;
        result.erase(std::remove_if(result.begin(), result.end(), ::ispunct), result.end());
        return result;
    }

    int main() {
        std::ifstream inputFile("tekstas.txt"); 
        std::map<std::string, std::vector<int>> wordOccurrences;
        std::map<std::string, int> wordCount;
        std::vector<std::string> lines;

        if (inputFile.is_open()) {
            std::string line;
            int lineNumber = 1;
            while (std::getline(inputFile, line)) {
                lines.push_back(line);
                std::istringstream iss(line);
                std::string word;
                while (iss >> word) {
                    word = removePunctuation(word);
                    wordOccurrences[word].push_back(lineNumber);
                    wordCount[word]++;
                }
                lineNumber++;
            }
            inputFile.close();
        } else {
            std::cerr << "Unable to open the file." << std::endl;
            return 1;
        }

        // Rūšiuojame žodžius pagal pasikartojimų skaičių
        std::vector<std::pair<std::string, int>> sortedWords(wordCount.begin(), wordCount.end());
        std::sort(sortedWords.begin(), sortedWords.end(), [](const auto &a, const auto &b) {
            return a.second > b.second;
        });

        // Išvedame rezultatus į failą
        std::ofstream outputFile("rezultatai.txt");
        if (outputFile.is_open()) {
            for (const auto &wordPair : sortedWords) {
                if (wordPair.second > 1) {
                    outputFile << wordPair.first << ": " << wordPair.second << " kartus" << std::endl;
                }
            }
            outputFile.close();
        } else {
            std::cerr << "Unable to create the output file." << std::endl;
            return 1;
        }

    // Išvedame cross-reference lentelę į failą
    std::ofstream crossRefFile("cross_reference.txt");
    if (crossRefFile.is_open()) {
        for (const auto &wordPair : wordOccurrences) {
            if (wordPair.second.size() > 1) {
                crossRefFile << wordPair.first << ": ";
                for (const auto &lineNumber : wordPair.second) {
                    crossRefFile << "line " << lineNumber << ", ";
                }
                crossRefFile.seekp(-2, std::ios_base::end); // Pašaliname paskutinį ", "
                crossRefFile << std::endl;
            }
        }
        crossRefFile.close();
    } else {
        std::cerr << "Unable to create the cross-reference file." << std::endl;
        return 1;
    }

    // Ištraukiame URL adresus iš teksto
    std::ifstream textFile("tekstas.txt"); // Nuskaitome tekstą iš naujo
    std::string text((std::istreambuf_iterator<char>(textFile)), std::istreambuf_iterator<char>());
    textFile.close();

    std::regex urlRegex("(https?://\\S+|www\\.\\S+)");
    std::smatch match;
    std::vector<std::string> urls;

    while (std::regex_search(text, match, urlRegex)) {
        urls.push_back(match[0]);
        text = match.suffix().str();
    }

    // Išvedame surastus URL'us
    std::cout << "URL addresses found in the text:" << std::endl;
    for (const auto &url : urls) {
        std::cout << url << std::endl;
    }

    return 0;
}
