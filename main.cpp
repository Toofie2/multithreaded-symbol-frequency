#include <iostream>
#include <string>
#include <stdlib.h>
#include <pthread.h>
#include <cstdint>
#include <vector>

struct SymbolResult{
    char symbol;
    std::string raw_base10;
    std::string raw_base2;
    std::string raw_base16;
    std::string le_base10;
    std::string le_base2;
    std::string le_base16;
    std::string be_base10;
    std::string be_base2;
    std::string be_base16;
};

struct Arguments {
    char targetChar;
    const std::string* fullMessage;
    SymbolResult* outBox;
};

//conversions
std::string reverseStr(std::string code){
    std::string rev = "";
    int n = code.length() - 1;
    while(n >= 0){
        rev.push_back(code[n]);
        n--;
    }
    return rev;
}

std::string toBaseTwo(uint16_t n){
    std::string baseTwo = "";
    while (n!=0){
        int remainder = n%2;
        baseTwo.push_back('0' + remainder);
        n = n/2;
    }
    while (baseTwo.length() < 16){
        baseTwo.push_back('0');
    }
    return reverseStr(baseTwo);
}

std::string toBaseTen(uint16_t n){
    if (n == 0) return "0";
    std::string baseTen = "";
    while(n != 0){
        int remainder = n%10;
        baseTen.push_back('0' + remainder);
        n = n/10;
    }
    return reverseStr(baseTen);
}

std::string toBaseSixteen(uint16_t n){
    std::string baseSixteen = "";
    const char hexChar[] = "0123456789ABCDEF";
    while (n!= 0){
        int remainder = n%16;
        baseSixteen.push_back(hexChar[remainder]);
        n = n/16;
    }
    while (baseSixteen.length() < 4){
        baseSixteen.push_back('0');
    }
    return reverseStr(baseSixteen);
}

uint16_t toLittleEndian(uint16_t n){
    uint16_t lowByte = n & 0xFF;
    uint16_t highByte = (n >> 8) & 0xFF;
    uint16_t swapped = (lowByte << 8) | highByte;
    return swapped;
}

//thread function
void*threadFunction(void* arg_ptr){
    struct Arguments*args = (struct Arguments*)arg_ptr;
    struct SymbolResult* symbol = args->outBox;
    symbol->symbol = args->targetChar;
    uint16_t count = 0;
    const std::string& msg = *(args->fullMessage);

    //get frequency of the character
    for(size_t i = 0; i < msg.length(); i ++){
        if(msg[i] == args->targetChar){
            count++;
        }
    }

    //make calculations
    uint16_t leCount = toLittleEndian(count);

    symbol->raw_base10 = toBaseTen(count);
    symbol->raw_base2  = toBaseTwo(count);
    symbol->raw_base16 = toBaseSixteen(count);
    
    symbol->le_base10  = toBaseTen(leCount);
    symbol->le_base2   = toBaseTwo(leCount);
    symbol->le_base16  = toBaseSixteen(leCount);
    
    symbol->be_base10  = toBaseTen(count);
    symbol->be_base2   = toBaseTwo(count);
    symbol->be_base16  = toBaseSixteen(count);

    return nullptr;
}

int main() {
    std::string rawVal;
    std::cin>> rawVal;
    std::vector<char> uniqueChar;

    //preserving one of each unique character
    for(size_t i = 0; i < rawVal.length(); i++){
        char currentLetter = rawVal[i];
        bool alreadyExists = false;
        
        for(size_t j = 0; j < uniqueChar.size(); j++) {
            if(uniqueChar[j] == currentLetter) {
                alreadyExists = true;
                break;
            }
        }
        if(!alreadyExists){
            uniqueChar.push_back(currentLetter);
        }
    }

    size_t n = uniqueChar.size();

    std::vector<pthread_t> tid(n);
    std::vector<Arguments> args(n);
    std::vector<SymbolResult> finalResults(n);

    //create threads for each character
    for(size_t i = 0; i < n; i++){
        args[i].targetChar = uniqueChar[i];
        args[i].fullMessage = &rawVal;
        args[i].outBox = &finalResults[i];
        
        pthread_create(&tid[i], nullptr, threadFunction, &args[i]);
    }

    //join each thread before printing
    for(size_t i = 0; i < n; i++){
        pthread_join(tid[i], nullptr);
    }

    //print results
    for(size_t i = 0;i < finalResults.size();i++){

        std::cout << "Symbol: " << finalResults[i].symbol << "\n";

        std::cout << "Frequency (raw) value: " << finalResults[i].raw_base10 << "\n";
        std::cout << "Frequency base 2 (raw) value: " << finalResults[i].raw_base2 << "\n";
        std::cout << "Frequency base 16 (raw) value: " << finalResults[i].raw_base16 << "\n";

        std::cout << "Frequency Little Endian base 10 value: " << finalResults[i].le_base10 << "\n";
        std::cout << "Frequency Little Endian base 2 value: " << finalResults[i].le_base2 << "\n";
        std::cout << "Frequency Little Endian base 16 value: " << finalResults[i].le_base16 << "\n";

        std::cout << "Frequency Big Endian base 10 value: " << finalResults[i].be_base10 << "\n";
        std::cout << "Frequency Big Endian base 2 value: " << finalResults[i].be_base2 << "\n";
        std::cout << "Frequency Big Endian base 16 value: " << finalResults[i].be_base16 << "\n";

        if(i<finalResults.size()-1){
            std::cout << "\n";
        }
    }
}
