#include "virtual_disc.h"
#include <fstream>
#include <cstring>
#include "boost/filesystem.hpp"
#include <filesystem>
#include <cmath>
#include <iomanip>

Virtual_file_system::Virtual_file_system() {};


Virtual_file_system::Virtual_file_system(std::string name, int disc_size){
    if (name.length() > DISC_NAME_SIZE){
        throw DiscNameTooLong();
    }
    super_block.disc_name = name;
    super_block.blocks_nr = disc_size;
    super_block.blocks_used = SYSTEM_BLOCKS_SIZE;
    super_block.free_blocks = super_block.blocks_nr - super_block.blocks_used;

    create_file_system(super_block.blocks_nr);
};

Virtual_file_system::~Virtual_file_system(){ };

void Virtual_file_system::create_file_system(unsigned blocks_num){
    if(super_block.blocks_nr <= SYSTEM_BLOCKS_SIZE){
        throw TooSmallMemorySize();
    }
    if(super_block.blocks_nr > MAX_DISC_SIZE){
        throw TooBigMemorySize(); 
    }

    //empty data block
    data_block_t data_block;
    for(unsigned i=0; i<DATA_BLOCK_SIZE; ++i){
        data_block.data[i] = '\0';
    }

    super_block.blocks_used = SYSTEM_BLOCKS_SIZE;

    std::fstream fs;
    //fill virtual disc with empty blocks
    fs.open((super_block.disc_name).c_str(), std::ios::out|std::ios::binary);
    for (unsigned i=0; i<super_block.blocks_nr; ++i){
        fs.write(data_block.data, DATA_BLOCK_SIZE);
    }
    fs.close();
};

unsigned Virtual_file_system::alloc(unsigned block_num){
    // allocate where put first data block
    if (iNodes.size() == 0){
        return SYSTEM_BLOCKS_SIZE;
    }
    else{
        return iNodes[iNodes.size()-1].end;
    }
};

void Virtual_file_system::upload_file(std::string file_name){
    // check if we the file name is unique
    if ( !file_name_free(file_name.c_str())){
        throw FileNameExists();
    }

    // info about new file
    std::string source_name = file_name;
    unsigned source_size = boost::filesystem::file_size(source_name);
    unsigned source_blocks_nr = floor(source_size/DATA_BLOCK_SIZE) + 1;

    if (source_blocks_nr > super_block.free_blocks){
        throw NotEnoughSpace();
    }
    
    super_block.free_blocks -= source_blocks_nr;
    super_block.blocks_used += source_blocks_nr;

    unsigned position = alloc(source_blocks_nr);
 
    char datablocks [source_blocks_nr][DATA_BLOCK_SIZE];

    std::ifstream ifstr;
    ifstr.open(source_name.c_str());
    for (unsigned i = 0; i < source_blocks_nr; ++i){
        ifstr.read(datablocks[i], DATA_BLOCK_SIZE);
    }

    std::fstream fstr;
    fstr.open((super_block.disc_name).c_str(), std::ios::in|std::ios::out|std::ios::binary);
    fstr.seekp(position * DATA_BLOCK_SIZE);

    for (unsigned i=0; i<source_blocks_nr; ++i){
        fstr.write(datablocks[i], DATA_BLOCK_SIZE);
    }

    for (unsigned i=0; i < source_blocks_nr; i++){
        for(unsigned j=0; j<DATA_BLOCK_SIZE; j++){
            datablocks[i][j] = '\0';
        }
    }

    iNode_t new_inode;
    new_inode.used = true;
    strcpy(new_inode.file_name, source_name.c_str());
    new_inode.file_size = source_size;
    new_inode.begin = position;
    new_inode.blocks = source_blocks_nr;
    new_inode.end = new_inode.begin + new_inode.blocks;
    iNodes.push_back(new_inode);

};

bool Virtual_file_system::file_name_free(std::string file_name){
    for(iNode_t iNode : iNodes){
        if (iNode.file_name == file_name){
            return false;
        }
    }
    return true;
};

void Virtual_file_system::ls(){
    std::cout << "[File name]" << std::setw(18)
              << "[Size in bytes]" << std::setw(18)
              << "[Block size]" << std::setw(18)
              << "[Block start]" << std::setw(18)
              << "[Block end]" << std::endl;

    for (iNode_t iNode : iNodes){
        std::cout << iNode.file_name << std::setw(18)
                  << iNode.file_size << std::setw(18)
                  << iNode.blocks << std::setw(18)
                  << iNode.begin << std::setw(18)
                  << iNode.end << std::endl;
    }


    std::cout << "\n\n";

    char info[super_block.blocks_nr];

    for (unsigned i=0; i<SYSTEM_BLOCKS_SIZE; i++){
        info[i] = 's';
    }

    for (unsigned i=SYSTEM_BLOCKS_SIZE; i<super_block.blocks_nr; i++){
        info[i] = '.';
    }

    for (unsigned i = 0; i < iNodes.size(); i++){
        for(unsigned j = iNodes[i].begin; j < iNodes[i].end; j++){
            info[j] = '#';
        }
    }

    float percent_used = (float)super_block.blocks_used/(float)super_block.blocks_nr;
    percent_used *= 100;

    std::cout << "Memory:\t\t[";

    for (char elem : info){
        std::cout << elem;
    }

    std::cout << "]" << "\t in usage: " << percent_used << "%\t" << (float)super_block.free_blocks*DATA_BLOCK_SIZE/1000 << "kB left" << std::endl;
    std::cout << "-----------------------" << std::endl;
};

void Virtual_file_system::remove_file(std::string file_name){
    iNode_t *iNode = NULL;

    unsigned counter = 0;
    for (iNode_t curr_iNode : iNodes){
        if (curr_iNode.file_name == file_name){
            iNode = &curr_iNode;
            break;
        }
        counter++;
    }

    if (iNode == NULL){
        throw FileNotExists(); 
    }

    unsigned offset = iNode->begin*DATA_BLOCK_SIZE;
    unsigned blocks_nr = iNode->blocks;

    super_block.blocks_used -= iNode->blocks;
    super_block.free_blocks += iNode->blocks;

    char datablocks [blocks_nr][DATA_BLOCK_SIZE];

    for (unsigned i = 0; i < blocks_nr; ++i){
        for(unsigned j=0; j<DATA_BLOCK_SIZE; ++j){
            datablocks[i][j] = '\0';
        }
    }

    std::fstream fstr;
    fstr.open((super_block.disc_name).c_str(), std::ios::in|std::ios::out|std::ios::binary);
    fstr.seekp(offset);

    for (unsigned i=0; i<blocks_nr; ++i){
        fstr.write(datablocks[i], DATA_BLOCK_SIZE);
    }

    fstr.close();

    iNodes.erase(iNodes.begin() + counter);
    
    unsigned idx;
    if (counter == 0){
        idx = 0;
    }
    else{
        idx = counter-1;
    }

    arrange(idx);

}

void Virtual_file_system::download_file(std::string file_name){
    iNode_t *iNode;

    for (iNode_t curr_iNode : iNodes){
        if (curr_iNode.file_name == file_name){
            iNode = &curr_iNode;
            break;
        }
    }

    if (iNode == NULL){
        throw FileNotExists(); 
    }

    
    unsigned offset = iNode->begin;
    offset = offset*DATA_BLOCK_SIZE;

    unsigned file_size = iNode->file_size;
    std::string new_name = iNode->file_name;
    new_name.resize(new_name.size() - 4);

    data_block_t data_block;

    std::ofstream ofstr;
    ofstr.open(new_name + "_d.txt");
    std::ifstream ifstr;
    ifstr.open(super_block.disc_name.c_str());

    // DLACZEGO TUTAJ InODE->BEGIN = 0?!!?!!?!?!?!/11111

    ifstr.seekg(offset);

    unsigned curr_block_size = DATA_BLOCK_SIZE;

    while(file_size > 0){
        if (file_size < DATA_BLOCK_SIZE){
            curr_block_size = file_size;
        }

        ifstr.read(data_block.data, curr_block_size);
        ofstr.write(data_block.data, curr_block_size);
        file_size -= curr_block_size;
    }

    ofstr.close();
    ifstr.close();

}

// get previous iNode
void Virtual_file_system::arrange(unsigned idx){
        bool wasIdxZero;
        if(idx == 0){
            wasIdxZero = true;
        }
        else{
            wasIdxZero = false;
        }

    for (unsigned i = idx; i<iNodes.size(); i++){
        unsigned free_block;
        iNode_t *iNode_to_move = NULL;
        if(wasIdxZero){
            if (i==0){
                free_block = SYSTEM_BLOCKS_SIZE;
            }
            else{
                free_block = iNodes[i-1].end;
            }
            iNode_to_move = &iNodes[i];
        }
        else{
            free_block = iNodes[i].end;
            iNode_to_move = &iNodes[i+1];
        }

        unsigned new_position = free_block * DATA_BLOCK_SIZE;
        unsigned blocks = iNode_to_move->blocks;
        unsigned node_position = iNode_to_move->begin * DATA_BLOCK_SIZE;

        char datablocks[blocks][DATA_BLOCK_SIZE];

        std::ifstream ifstr;
        ifstr.open(super_block.disc_name.c_str());
        ifstr.seekg(node_position);

        for(unsigned j = 0; j < blocks; j++){
            ifstr.read(datablocks[j], DATA_BLOCK_SIZE);
        }

        std::fstream fstr;
        fstr.open(super_block.disc_name.c_str(), std::ios::in|std::ios::out|std::ios::binary);
        fstr.seekp(new_position);
        for(unsigned j=0; j<blocks; j++){
            fstr.write(datablocks[j], DATA_BLOCK_SIZE);
        }

        // czysc datablock
        for (unsigned j = 0; j < blocks; ++j){
            for(unsigned k=0; k<DATA_BLOCK_SIZE; ++k){
            datablocks[j][k] = '\0';
            }
        }

        iNode_to_move->begin = free_block;
        iNode_to_move->end = free_block + iNode_to_move->blocks;

    }
}

void Virtual_file_system::close(){
    remove(super_block.disc_name.c_str());
}
