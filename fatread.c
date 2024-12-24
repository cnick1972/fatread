#include <stdio.h>
#include <stdint.h>

typedef struct {
    uint8_t     DriveAttributes;
    uint8_t     CHSAddress[3];
    uint8_t     PartitionType;
    uint8_t     CHSAddressLastPartSector[3];
    uint32_t    LBAPartitionStart;
    uint32_t    NumberOfSectors;

} __attribute__((packed)) PartitiontableEntry;

typedef struct {
    uint8_t     MBRBootStrap[440];
    uint32_t    DiskID;
    uint16_t    reserved;
    PartitiontableEntry Part1;
    PartitiontableEntry Part2;
    PartitiontableEntry Part3;
    PartitiontableEntry Part4;
    uint16_t    signature;

} __attribute__((packed)) MBR;

typedef struct {
    uint8_t     jmp[3];
    uint8_t     OEM_identifer[8];
    uint16_t    BytesPerSector;
    uint8_t     SectorsPerCluster;
    uint16_t    ReservedSectors;
    uint8_t     NumberOfFats;
    uint16_t    RootDirectories;
    uint16_t    TotalSectors;
    uint8_t     MediaDescriptor;
    uint16_t    SectorsPerFat;
    uint16_t    SectorsPerTrack;
    uint16_t    Numberofheads;
    uint32_t    HiddenSectors;
    uint32_t    LargeSectorCount;

} __attribute__((packed))fatstruct;

int main() {
    FILE* file;

    fatstruct fs;

    MBR mbr;
    size_t i;

    file = fopen("disk.img", "r");

    if(file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    size_t bytesRead = fread(&mbr, 1, sizeof(mbr), file);

    fseek(file, 1048576, SEEK_SET);
    bytesRead = fread(&fs, 1, sizeof(fs), file);




    printf("DISKID: 0x%x\n", mbr.DiskID);
    printf("Drive attr 0x%x\n", mbr.Part1.DriveAttributes);
    printf("Boot Signature 0x%x\n", mbr.signature);
    printf("LBA address: 0x%x\n", mbr.Part1.LBAPartitionStart);

    printf("%x %x %x \n", mbr.Part1.CHSAddress[0], mbr.Part1.CHSAddress[1], mbr.Part1.CHSAddress[2]);

    printf("Starting Head: %d\n", mbr.Part1.CHSAddress[0]);
    uint16_t cylinders = (((uint16_t)mbr.Part1.CHSAddress[1] << 4) & 0b0000110000000000) + mbr.Part1.CHSAddress[2];
    printf("Starting Cylinder: %d\n", cylinders);
    uint8_t sectors = mbr.Part1.CHSAddress[1] & 0x3f;
    printf("Starting Sector: %d\n", sectors);
    // display first 3 bytes
    printf("Jmp block: ");
    for(i = 0; i < 3; i++) {
        printf("0x%x ", fs.jmp[i]);
    }
    printf("\nOEM Block: ");
    for(i = 0; i < sizeof(fs.OEM_identifer); i++) {
        printf("%c", fs.OEM_identifer[i]);
    }
    printf("\nBytes per Sector: %d", fs.BytesPerSector);
    printf("\nSectors Per Cluster: %d", fs.SectorsPerCluster);
    printf("\nReserved Sectors: %d", fs.ReservedSectors);
    printf("\nNumber of FATs: %d", fs.NumberOfFats);
    printf("\nRoot Directories: %d", fs.RootDirectories);
    printf("\nTotal Sectors: %d", fs.TotalSectors);
    printf("\nMedia Descriptor: 0x%x", fs.MediaDescriptor);
    printf("\nSectors per Fat: %d", fs.SectorsPerFat);
    printf("\nSectors per track: %d", fs.SectorsPerTrack);
    printf("\nNumber of Heads: %d", fs.Numberofheads);
    printf("\nHidden Sectors: %d", fs.HiddenSectors);
    printf("\nLarge Sector Count: %d\n", fs.LargeSectorCount);

    fclose(file);

    return 0;
}