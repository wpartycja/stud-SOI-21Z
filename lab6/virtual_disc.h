#include <iostream>
#include <vector>
#include <exception>


#define MAX_FILE_NAME_SIZE 40


class Virtual_file_system{
    private:
        static const unsigned DATA_BLOCK_SIZE = 1024; // 1 kB
        static const unsigned MAX_DISC_SIZE = 47;
        static const unsigned DISC_NAME_SIZE = 44;
        static const unsigned SYSTEM_BLOCKS_SIZE = 3; // bo przy max 47 plikach tyle pamieci trzeba na systemowe
                                                      // 48 + 60*47 = 2 868


        struct iNode_t{
            bool used;
            char file_name[MAX_FILE_NAME_SIZE];
            unsigned file_size;
            unsigned begin;
            unsigned end;
            unsigned blocks;
        }; // size of(iNode_t) = 60

        struct data_block_t{
            char data[DATA_BLOCK_SIZE];
        };

        struct iNode_t;
        struct data_block_t;

        std::vector<iNode_t> iNodes;
        bool file_name_free(std::string file_name);
        void arrange(unsigned idx);

    public:

        class NotEnoughSpace: public std::exception{
            const char* what() const throw(){
                return "Unfortunately virtual file system is full :(";
            }
        };

        class TooSmallMemorySize: public std::exception{
            const char* what() const throw(){
                return "Sorry, size od virtual file system should be bigger";
            }
        };

        class TooBigMemorySize: public std::exception{
            const char* what() const throw(){
                return "Oops, that's too much! I can't handle this :)";
            }
        };

        class DiscNameTooLong: public std::exception{
            const char* what() const throw(){
                return "Disk name should be shorter";
            }
        };

        class FileNameExists: public std::exception{
            const char* what() const throw(){
                return "File of this name already exists";
            }
        };

        class FileNotExists: public std::exception{
            const char* what() const throw(){
                return "File of this name doesn't exist";
            }
        };

        class FileNameTooLong: public std::exception{
            const char* what() const throw(){
                return "File name too long:((";
            }
        };

        Virtual_file_system();
        Virtual_file_system(std::string name, int disc_size);
        ~Virtual_file_system();

        void upload_file(std::string file_name);
        void download_file(std::string file_name);
        void list_files();
        void remove_file(std::string file_name);
        void show_info();
        void ls();
        void create_file_system(unsigned blocks_num);
        unsigned alloc(unsigned block_nr);
        void close();
        

        struct super_block_t{
            std::string disc_name;
            unsigned blocks_nr;
            unsigned blocks_used;
            unsigned free_blocks;
        }; // size_of(super_block_t) = 48 

        struct super_block_t super_block;
    
};
