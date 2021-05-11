#include "FS.h"
#include "SD_MMC.h"

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    #ifdef debug
    Serial.printf("Listing directory: %s\n", dirname);
    #endif

    File root = fs.open(dirname);
    if(!root){
        #ifdef debug
        Serial.println("Failed to open directory");
        #endif
        return;
    }
    if(!root.isDirectory()){
        #ifdef debug
        Serial.println("Not a directory");
        #endif
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            #ifdef debug
            Serial.print("  DIR : ");
            #endif
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            #ifdef debug
            Serial.print("  FILE: ");
            #endif
            Serial.print(file.name());
            #ifdef debug
            Serial.print("  SIZE: ");
            #endif
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    #ifdef debug
    Serial.printf("Creating Dir: %s\n", path);
    #endif
    if(fs.mkdir(path)){
        #ifdef debug
        Serial.println("Dir created");
        #endif
    } else {
        #ifdef debug
        Serial.println("mkdir failed");
        #endif
    }
}

void removeDir(fs::FS &fs, const char * path){
    #ifdef debug
    Serial.printf("Removing Dir: %s\n", path);
    #endif
    if(fs.rmdir(path)){
        #ifdef debug
        Serial.println("Dir removed");
        #endif
    } else {
        #ifdef debug
        Serial.println("rmdir failed");
        #endif
    }
}

void readFile(fs::FS &fs, const char * path){
    #ifdef debug
    Serial.printf("Reading file: %s\n", path);
#endif
    File file = fs.open(path);
    if(!file){
        #ifdef debug
        Serial.println("Failed to open file for reading");
        #endif
        return;
    }
#ifdef debug
    Serial.print("Read from file: ");
    #endif
    while(file.available()){
        Serial.write(file.read());
    }
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    #ifdef debug
    Serial.printf("Writing file: %s\n", path);
#endif
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        #ifdef debug
        Serial.println("Failed to open file for writing");
        #endif
        return;
    }
    if(file.print(message)){
        #ifdef debug
        Serial.println("File written");
        #endif
    } else {
        #ifdef debug
        Serial.println("Write failed");
        #endif
    }
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

void setup_sdcard() {
    if(!SD_MMC.begin("/sdcard", true, false)){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD_MMC.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD_MMC card attached");
        return;
    }

    Serial.print("SD_MMC Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);
    /*
    listDir(SD_MMC, "/", 0);
    createDir(SD_MMC, "/mydir");
    listDir(SD_MMC, "/", 0);
    removeDir(SD_MMC, "/mydir");
    listDir(SD_MMC, "/", 2);
    writeFile(SD_MMC, "/hello.txt", "Hello ");
    appendFile(SD_MMC, "/hello.txt", "World!\n");
    readFile(SD_MMC, "/hello.txt");
    deleteFile(SD_MMC, "/foo.txt");
    renameFile(SD_MMC, "/hello.txt", "/foo.txt");
    readFile(SD_MMC, "/foo.txt");
    */
   /*
      xTaskCreate(
                    Task1code,  
                    "Task1",     
                    10000,     
                    NULL,        
                    1,       
                    &Task1);      
                    */
    //Serial.printf("Total space: %lluMB\n", SD_MMC.totalBytes() / (1024 * 1024));
    //Serial.printf("Used space: %lluMB\n", SD_MMC.usedBytes() / (1024 * 1024));
}
