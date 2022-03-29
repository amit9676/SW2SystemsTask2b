#include "Notebook.hpp"
#include <string>
#include <stdexcept>
#include "Direction.hpp"
#include <iostream>
#include <map>
using namespace std;
using namespace ariel;


void errorThrower(int page, int row, int col){ // basic errorThrower for page,row,col values
    const int qq = 99;
    const int o = 0;
    if(page < o){
        throw std::invalid_argument("page value must be positive or 0");
    }
    if(row < o){
        throw std::invalid_argument("row value must be positive or 0");
    }
    if(col < o || col > qq){
        throw std::invalid_argument("column value must be between 0 and 99");
    }
    
}

void errorThrowerWrite(string str){
    const char maxAscii = '~';
    const char space = ' ';
    for(int i = 0; i < str.length(); i++){
        char c = str.at(unsigned(i));
        if(c < space || c >= maxAscii){
            throw std::invalid_argument("invalid ascii");
        }
    }
}

//checking boundries for read and erase functions
//upper bound exists only for horizontal actions/
void errorThrowerReadErase(int s, Direction direction){
    const int qq = 100;
    if(s < 0 || (s > qq && direction == Direction::Horizontal)){
        throw std::invalid_argument("length value must be between 0 and 99");
    }
}

string lineWriter(string &original, int start, string newWriting){
    //recieve all line, write the relevant part
    for(int i = start; i < unsigned(start) + newWriting.length(); i++){
        original[unsigned(i)] = newWriting[unsigned(i-start)];
    }
    return original; 
}

string lineReader(string original, int start, int length){
    
    //recieve all line, display relevant part
    string result;
    for(int i = start; i < start + length; i++){
        result += original[unsigned(i)];
    }
    return result; 
}

bool validString(string input){//check some space in notebook, see if there is already written area in it.
    for(int i = 0; i < input.length(); i++){
        if(input[unsigned(i)] != '_'){
            return false;
        }
    }
    return true;
    

}

void createPage(int page, Notebook nbook){
    //if page does not exists - create it (made with finding key on hashmap)
    if(nbook.notebook.find(page) == nbook.notebook.end()){
        map<int,string> rowsForPages;
        nbook.notebook.insert(pair<int,map<int, string>>(page,rowsForPages));
    }
}

void createRow(int page, int row, Notebook *nbook){
    //if row does not exists - create it (made with finding key on hashmap)
    // note - function assumes page exists
    if(nbook->notebook[page].find(row) == nbook->notebook[page].end()){
        const int ooo = 100;
        string p(ooo,'_');
        nbook->notebook[page].insert(pair<int,string>(row,p)); // create empty row
    }
    
}

void Notebook::write(int page, int row, int col, Direction direction, string str){
    //check if the input is valid
    errorThrower(page,row,col);
    errorThrowerWrite(str);

    //if desired page doesnt exists - create it
    createPage(page,*this);
    

    //if requested writing is horizontal
    if(direction == Direction::Horizontal){
        createRow(page,row,this);
        const int ooo = 100;
        if((int)((unsigned int)(col)+str.length()) >= ooo){
            //writing extends page limit
            throw std::invalid_argument("writing exeeding row limits");
            return;
        }
        if(!validString(lineReader(this->notebook[page][row],col,str.length()))){
            //there is already written space here, abort! (throw some error)
            
            throw std::invalid_argument("space is already written or erased, you cant write here!");
            return;
        }

        lineWriter(this->notebook[page][row],col,str); //write
    }
    else if(direction == Direction::Vertical){ //if requested writing is vertical
        for(int i = 0; i < (int)str.length(); i++){
            createRow(page,row+i,this);
            if(!validString(lineReader(this->notebook[page][row+i],col,1))){
                //if on column spot of each used row for vertical writing - there is already used space:
                throw std::invalid_argument("space is already written or erased, you cant write here!");
                return;
            }
        }

        for(int i = 0; i < (int)str.length(); i++){
            //create line, if all pre-requisites exist
            string p(1,str[unsigned(i)]); //get the right character to place in every line in the right location
            lineWriter(this->notebook[page][row+i],col,p);
        }
        
    }
    
}

string Notebook::read(int page, int row, int col, Direction direction , int s){
    //check for errors
    errorThrower(page,row,col);
    errorThrowerReadErase(s,direction);

    //create page if it doesnt exists
    createPage(page,*this);
    

    string result;
    //if reading is Horizontal
    if(direction == Direction::Horizontal){
        createRow(page,row,this);
        const int ooo = 100;
        if(col + s - 1 >= ooo){
            //throw error because reading exeeding page boundries
            throw std::invalid_argument("writing exeeding row limits");
        }
        result = lineReader(notebook[page][row],col,s);
    }
    else if(direction == Direction::Vertical){ //if the reading request if vertical
        for(int i = row; i < row+s; i++){
            createRow(page,i,this);
            result += lineReader(notebook[page][i],col,1); // the one is because from each line we read
            //single character only
        }
    }
    return result;

}

void Notebook::erase(int page, int row, int col, Direction direction, int s){
    //input check
    errorThrower(page,row,col);
    errorThrowerReadErase(s,direction);
    createPage(page,*this);
    

    //erasing function works similarly to writing function, only the 'writing' input is '~' only.
    //and unlike writing function it can overwrite any character
    string erasing;
    for(int i = 0; i < s; i++){
        erasing += '~';
    }
    if(direction == Direction::Horizontal){
        createRow(page,row,this);
        errorThrowerReadErase(col+s,direction);
        lineWriter(this->notebook[page][row],col,erasing);
    }
    else if(direction == Direction::Vertical){
        for(int i = 0; i < s; i++){
            createRow(page,row+i,this);
            string p(1,'~');
            lineWriter(this->notebook[page][row+i],col,p);
        }
    }
    

}

//display existing rows only in selected page
void Notebook::show(int page){
    if(page < 0){
        throw std::invalid_argument("paged index is negative");
        return;
    }
    createPage(page,*this);
    for ( const auto &myPair : this->notebook[page] ) {
        std::cout << myPair.first << ": " << myPair.second <<"\n";
    }
}