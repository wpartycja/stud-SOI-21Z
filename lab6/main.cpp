#include "virtual_disc.cpp"
#include <cstring>

void help_message(){
    std::cout << "Usage:\nupload - to upload file to virtual disk" << std::endl
              << "download - to download from virtual disk" << std::endl
              << "remove - to remove file file from virtual disc" << std::endl
              << "delete - to delete virtual disk" << std::endl
              << "exit - to exit program" << std::endl;
}


int main(int argc, char *argv[]){

        Virtual_file_system FS;
        std::string input;
        while(true){
            std::cout << "~/minixdeveloper/$ ";
            std::cin >> input;
            try{

                if (input == "new"){
                    
                    std::cout << "Name of your disc: ";
                    std::string disc_name;

                    while(true){
                        std::cin >> disc_name;
                        if (disc_name.size() > MAX_FILE_NAME_SIZE){
                            std::cout << "There should be maximum " << MAX_FILE_NAME_SIZE << " letters. Try again" << std::endl;
                        }
                        else{
                            break;
                        }
                    }

                    std::cout << "\nEnter size of your file system (how many blocks): ";
                    float file_sys_size;
                    std::cin >> file_sys_size;
                    
                    FS = Virtual_file_system(disc_name, file_sys_size);
                    break;
                }
                else{
                    std::cout << "At first you have create your disc,\nuse: new" << std::endl;
                }
            }
            catch (std::exception& e){
                std::cout << e.what() << std::endl;
            }
        }


    bool exit = false;
    while(!exit){
        std::cout << "~/minixdeveloper/$ ";
        std::string input;
        std::cin >> input;
        try{
            if (input == "ls"){
                FS.ls();
            }
            else if (input == "upload"){
                std::string file_name;
                std::cout << "Name of file: ";
                std::cin >> file_name;
                FS.upload_file(file_name);
            }
            else if (input == "download"){
                std::string file_name;
                std::cout << "Name of file: ";
                std::cin >> file_name;
                FS.download_file(file_name);
            }
            else if (input == "remove"){
                std::string file_name;
                std::cout << "Name of file: ";
                std::cin >> file_name;
                FS.remove_file(file_name);
            }
            else if (input == "delete"){
                FS.close();
                break;
            }
            else if (input == "exit"){
                break;
            }
            else if (input == "help"){
                help_message();
            }
            else{
                std::cout << "Unknown command. Use: help" << std::endl;
            }
        } catch (std::exception& e){
            std::cout << e.what() << std::endl;
        }
     }
    return 0;

}
