/*
    NAME : Manmeet Mann
    INSTITUTE STUDENT ID : 1087101
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

/* Part A  Structs */

typedef struct FILE_INFO
{
    char *name;
    char *path;
} file_info;

typedef struct DIRS
{
    char *name;
    int num_files;
    int num_subdirs;
    file_info *files_in_dir;
    file_info *sub_dirs;
} directory;

typedef struct DIR_LEVS
{
    int num_dirs;
    directory *directories;
} dir_lev;

/* Part B  Structs */

typedef struct inode_fileinfo
{
    char *name;
    int inode_num;
    int size; /* Bytes */
    int blocks;
    int division;
} inode_file_info;

typedef struct INODE_DIRS
{
    char *name;
    int num_files;
    int num_subdirs;
    inode_file_info *files_in_dir;
    inode_file_info *sub_dirs;
} inode_directory;

typedef struct INODE_DIR_LEVS
{
    int num_dirs;
    inode_directory *directories;
} inode_dir_lev;

/* ---------------------------------------------------------------------------------------  */

// void delete_levels(dir_lev *array, int size)
// {
//     for (int i = 0; i < size; i++)
//     {
//         delete_fileinfo(array[i].directories);
//         delete_fileinfo(array[i].files);
//     }
// }

dir_lev *hierarchy;
inode_dir_lev *Ihierarchy;

char *days[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

int highest_level = -1;

int max(int a, int b)
{
    return (a > b ? a : b);
}

/* Sorting on basis of Alphabetical order */

void merge_alphabetically(file_info *a, int l, int mid, int h)
{
    int i, j, k;
    i = k = l;
    j = mid;
    file_info b[h + 1];
    for (int z = 0; z < h + 1; z++)
    {
        b[z].name = (char *)malloc(500 * sizeof(char));
        strcpy(b[z].name, "");
        b[z].path = (char *)malloc(500 * sizeof(char));
        strcpy(b[z].path, "");
    }

    while ((i <= mid - 1) && (j <= h))
    {
        if (strcmp(a[i].name, a[j].name) < 0)
        {
            strcpy(b[k].name, a[i].name);
            strcpy(b[k++].path, a[i++].path);
        }
        else
        {
            strcpy(b[k].name, a[j].name);
            strcpy(b[k++].path, a[j++].path);
        }
    }
    while (i <= mid - 1)
    {
        strcpy(b[k].name, a[i].name);
        strcpy(b[k++].path, a[i++].path);
    }
    while (j <= h)
    {
        strcpy(b[k].name, a[j].name);
        strcpy(b[k++].path, a[j++].path);
    }

    for (i = l; i <= h; i++)
    {
        strcpy(a[i].name, b[i].name);
        strcpy(a[i].path, b[i].path);
    }
    for (int z = 0; z < h + 1; z++)
    {
        free(b[z].name);
        free(b[z].path);
    }
}

void merge_sort_alphabetically(file_info *a, int n, int l, int r)
{
    int mid = (l + r) / 2;
    if (l < r)
    {
        merge_sort_alphabetically(a, n, l, mid);
        merge_sort_alphabetically(a, n, mid + 1, r);
        merge_alphabetically(a, l, mid + 1, r);
    }
    return;
}

void merge_levels(directory *a, int l, int mid, int h)
{
    int i, j, k;
    i = k = l;
    j = mid;
    directory b[h + 1];
    for (int z = 0; z < h + 1; z++)
    {
        b[z].name = NULL;
        b[z].files_in_dir = NULL;
        b[z].sub_dirs = NULL;
        b[z].num_files = 0;
        b[z].num_subdirs = 0;
    }

    while ((i <= mid - 1) && (j <= h))
    {
        if (strcmp(a[i].name, a[j].name) < 0)
        {
            b[k].name = a[i].name;
            b[k].files_in_dir = a[i].files_in_dir;
            b[k].sub_dirs = a[i].sub_dirs;
            b[k].num_files = a[i].num_files;
            b[k++].num_subdirs = a[i++].num_subdirs;
        }
        else
        {
            b[k].name = a[j].name;
            b[k].files_in_dir = a[j].files_in_dir;
            b[k].sub_dirs = a[j].sub_dirs;
            b[k].num_files = a[j].num_files;
            b[k++].num_subdirs = a[j++].num_subdirs;
        }
    }
    while (i <= mid - 1)
    {
        b[k].name = a[i].name;
        b[k].files_in_dir = a[i].files_in_dir;
        b[k].sub_dirs = a[i].sub_dirs;
        b[k].num_files = a[i].num_files;
        b[k++].num_subdirs = a[i++].num_subdirs;
    }
    while (j <= h)
    {
        b[k].name = a[j].name;
        b[k].files_in_dir = a[j].files_in_dir;
        b[k].sub_dirs = a[j].sub_dirs;
        b[k].num_files = a[j].num_files;
        b[k++].num_subdirs = a[j++].num_subdirs;
    }

    for (i = l; i <= h; i++)
    {
        a[i].name = b[i].name;
        a[i].files_in_dir = b[i].files_in_dir;
        a[i].sub_dirs = b[i].sub_dirs;
        a[i].num_files = b[i].num_files;
        a[i].num_subdirs = b[i].num_subdirs;
    }

    for (int z = 0; z < h + 1; z++)
    {
        b[z].name = NULL;
        b[z].files_in_dir = NULL;
        b[z].sub_dirs = NULL;
        b[z].num_files = 0;
        b[z].num_subdirs = 0;
    }
}

void mergesort_levels(directory *a, int n, int l, int r)
{
    int mid = (l + r) / 2;
    if (l < r)
    {
        mergesort_levels(a, n, l, mid);
        mergesort_levels(a, n, mid + 1, r);
        merge_levels(a, l, mid + 1, r);
    }
    return;
}

void listTREEinfo(const char *path, char *basename, int level)
{
    highest_level = max(highest_level, (level));
    struct dirent *dp;
    DIR *dir = opendir(path);

    /* base condition */
    if (!dir)
    {
        return;
    }

    while ((dp = readdir(dir)) != NULL)
    {
        if ((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0))
        {
            char curr_name[1000], new_path[10000];
            strcpy(new_path, path);
            strcat(new_path, "/");
            strcat(new_path, dp->d_name);
            strcpy(curr_name, dp->d_name);


            struct stat fileStat;
            if (stat(new_path, &fileStat) < 0)
                continue;

            if (S_ISDIR(fileStat.st_mode)) /* Its a Directory */
            {
                int idx = level - 1;

                int itr = hierarchy[idx].num_dirs;
                if (idx == 0)
                {
                    itr--;
                }

                int itr_inside = hierarchy[idx].directories[itr].num_subdirs;

                strcpy((hierarchy[idx].directories[itr].sub_dirs[itr_inside].name), curr_name);
                strcpy((hierarchy[idx].directories[itr].sub_dirs[itr_inside].path), new_path);
                strcpy((hierarchy[level].directories[hierarchy[level].num_dirs].name), curr_name);

                hierarchy[idx].directories[itr].num_subdirs++;

                listTREEinfo(new_path, curr_name, level + 1);

                hierarchy[level].num_dirs++;
            }
            else /* Its a File */
            {
                int idx = level - 1;
                int itr = hierarchy[idx].num_dirs;

                if (idx == 0)
                {
                    itr--;
                }
                int itr_inside = hierarchy[idx].directories[itr].num_files;

                strcpy(hierarchy[idx].directories[itr].files_in_dir[itr_inside].name, curr_name);
                strcpy(hierarchy[idx].directories[itr].files_in_dir[itr_inside].path, new_path);

                hierarchy[idx].directories[itr].num_files++;
            }
        }
    }

    closedir(dir);
}

/* =----------------------------------------------------------------------------------------------------------------= */

/* PART B FUNCTIONS */

void merge_inodes(inode_file_info *a, int l, int mid, int h)
{
    int i, j, k;
    i = k = l;
    j = mid;
    inode_file_info b[h + 1];

    for (int z = 0; z < h + 1; z++)
    {
        b[z].name = (char *)malloc(1000);
        b[z].size = 0;
        b[z].blocks = 0;
        b[z].inode_num = 0;
        b[z].division = 0;
    }

    while ((i <= mid - 1) && (j <= h))
    {
        if (a[i].inode_num < a[j].inode_num)
        {
            strcpy(b[k].name, a[i].name);
            b[k].size = a[i].size;
            b[k].blocks = a[i].blocks;
            b[k].inode_num = a[i].inode_num;
            b[k++].division = a[i++].division;
        }
        else
        {
            strcpy(b[k].name, a[j].name);
            b[k].size = a[j].size;
            b[k].blocks = a[j].blocks;
            b[k].inode_num = a[j].inode_num;
            b[k++].division = a[j++].division;
        }
    }
    while (i <= mid - 1)
    {
        strcpy(b[k].name, a[i].name);
        b[k].size = a[i].size;
        b[k].blocks = a[i].blocks;
        b[k].inode_num = a[i].inode_num;
        b[k++].division = a[i++].division;
    }
    while (j <= h)
    {
        strcpy(b[k].name, a[j].name);
        b[k].size = a[j].size;
        b[k].blocks = a[j].blocks;
        b[k].inode_num = a[j].inode_num;
        b[k++].division = a[j++].division;
    }

    for (i = l; i <= h; i++)
    {
        strcpy(a[i].name, b[i].name);
        a[i].size = b[i].size;
        a[i].blocks = b[i].blocks;
        a[i].inode_num = b[i].inode_num;
        a[i].division = b[i].division;
    }

    for (int z = 0; z < h + 1; z++)
    {
        free(b[z].name);
    }
}

void merge_sort_inode(inode_file_info *a, int n, int l, int r)
{
    int mid = (l + r) / 2;
    if (l < r)
    {
        merge_sort_inode(a, n, l, mid);
        merge_sort_inode(a, n, mid + 1, r);
        merge_inodes(a, l, mid + 1, r);
    }
    return;
}

void listINODEinfo(const char *path, char *basename, int level)
{
    highest_level = max(highest_level, (level));
    struct dirent *dp;
    DIR *dir = opendir(path);

    /* base condition */
    if (!dir)
    {
        return;
    }

    while ((dp = readdir(dir)) != NULL)
    {
        if ((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0))
        {
            char curr_name[1000], new_path[10000];
            strcpy(new_path, path);
            strcat(new_path, "/");
            strcat(new_path, dp->d_name);
            strcpy(curr_name, dp->d_name);

            struct stat fileStat;
            if (stat(new_path, &fileStat) < 0)
                continue;

            if (S_ISDIR(fileStat.st_mode)) /* Its a Directory */
            {
                int idx = level - 1;

                int itr = Ihierarchy[idx].num_dirs;
                if (idx == 0)
                {
                    itr--;
                }

                int itr_inside = Ihierarchy[idx].directories[itr].num_subdirs;

                strcpy((Ihierarchy[idx].directories[itr].sub_dirs[itr_inside].name), curr_name);

                strcpy((Ihierarchy[level].directories[Ihierarchy[level].num_dirs].name), curr_name);

                Ihierarchy[idx].directories[itr].sub_dirs[itr_inside].size = fileStat.st_size;
                Ihierarchy[idx].directories[itr].sub_dirs[itr_inside].inode_num = fileStat.st_ino;

                Ihierarchy[idx].directories[itr].sub_dirs[itr_inside].blocks = fileStat.st_blocks;
                Ihierarchy[idx].directories[itr].sub_dirs[itr_inside].division = ceil(((Ihierarchy[idx].directories[itr].sub_dirs[itr_inside].size) / (512.0)));
                Ihierarchy[idx].directories[itr].num_subdirs++;

                listINODEinfo(new_path, curr_name, level + 1);

                Ihierarchy[level].num_dirs++;
            }
            else /* Its a File */
            {
                int idx = level - 1;
                int itr = Ihierarchy[idx].num_dirs;

                if (idx == 0)
                {
                    itr--;
                }
                int itr_inside = Ihierarchy[idx].directories[itr].num_files;

                strcpy(Ihierarchy[idx].directories[itr].files_in_dir[itr_inside].name, curr_name);

                Ihierarchy[idx].directories[itr].files_in_dir[itr_inside].size = fileStat.st_size;
                Ihierarchy[idx].directories[itr].files_in_dir[itr_inside].inode_num = fileStat.st_ino;
                Ihierarchy[idx].directories[itr].files_in_dir[itr_inside].blocks = fileStat.st_blocks;
                Ihierarchy[idx].directories[itr].files_in_dir[itr_inside].division = ceil(((Ihierarchy[idx].directories[itr].files_in_dir[itr_inside].size * 1.0) / (512.0)));

                Ihierarchy[idx].directories[itr].num_files++;
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Too Few Arguments.\n");
        return 0;
    }

    if (argc > 3)
    {
        printf("Too Many Arguments.\n");
        return 0;
    }

    char *method, *path;
    for (int i = 1; i < argc; i++)
    {
        if (i == 1)
        {
            method = argv[i];
        }
        else
        {
            path = argv[i];
        }
    }

    if (strcmp(method, "-tree") == 0)
    {

        hierarchy = (dir_lev *)malloc(100 * sizeof(dir_lev));
        for (int i = 0; i < 100; i++)
        {
            hierarchy[i].num_dirs = 0;
            hierarchy[i].directories = (directory *)malloc(200 * sizeof(directory));

            for (int j = 0; j < 200; j++)
            {
                hierarchy[i].directories[j].num_files = 0;
                hierarchy[i].directories[j].num_subdirs = 0;

                hierarchy[i].directories[j].name = (char *)malloc(200 * sizeof(char));
                strcpy(hierarchy[i].directories[j].name, "");

                hierarchy[i].directories[j].files_in_dir = (file_info *)malloc(200 * sizeof(file_info));
                hierarchy[i].directories[j].sub_dirs = (file_info *)malloc(200 * sizeof(file_info));

                for (int k = 0; k < 200; k++)
                {

                    hierarchy[i].directories[j].files_in_dir[k].name = (char *)malloc(200 * sizeof(char));
                    strcpy(hierarchy[i].directories[j].files_in_dir[k].name, "");
                    hierarchy[i].directories[j].files_in_dir[k].path = (char *)malloc(200 * sizeof(char));
                    strcpy(hierarchy[i].directories[j].files_in_dir[k].path, "");
                }
                for (int k = 0; k < 200; k++)
                {

                    hierarchy[i].directories[j].sub_dirs[k].name = (char *)malloc(200 * sizeof(char));
                    strcpy(hierarchy[i].directories[j].sub_dirs[k].name, "");
                    hierarchy[i].directories[j].sub_dirs[k].path = (char *)malloc(200 * sizeof(char));
                    strcpy(hierarchy[i].directories[j].sub_dirs[k].path, "");
                }
            }
        }

        strcpy(hierarchy[0].directories[0].name, path);
        hierarchy[0].num_dirs = 1;

        int level = 1;


        listTREEinfo(path, path, level);


        for (int i = 0; i < highest_level; i++)
        {
            for (int j = 0; j < hierarchy[i].num_dirs; j++)
            {
                merge_sort_alphabetically(hierarchy[i].directories[j].files_in_dir, hierarchy[i].directories[j].num_files, 0, hierarchy[i].directories[j].num_files - 1);
                merge_sort_alphabetically(hierarchy[i].directories[j].sub_dirs, hierarchy[i].directories[j].num_subdirs, 0, hierarchy[i].directories[j].num_subdirs - 1);
            }
            mergesort_levels(hierarchy[i].directories, hierarchy[i].num_dirs, 0, hierarchy[i].num_dirs - 1);
        }
        printf("File System Report: Tree Directory Structure\n\n");

        for (int i = 0; i < highest_level; i++)
        {
            for (int j = 0; j < hierarchy[i].num_dirs; j++)
            {
                if (hierarchy[i].directories[j].num_subdirs > 0 || hierarchy[i].directories[j].num_files > 0)
                    printf("Level %d : %s\n", (i + 1), hierarchy[i].directories[j].name);

                if (hierarchy[i].directories[j].num_subdirs > 0)
                {
                    printf("Directories\n");
                    for (int k = 0; k < hierarchy[i].directories[j].num_subdirs; k++)
                    {
                        struct stat fileStat;
                        if (stat(hierarchy[i].directories[j].sub_dirs[k].path, &fileStat) < 0)
                            continue;

                        char *temp = (char *)malloc(1000);

                        struct passwd *pwd;
                        if ((pwd = getpwuid(fileStat.st_uid)) != NULL)
                        {
                            strcpy(temp, pwd->pw_name);
                        }
                        else
                        {
                            strcpy(temp, "");
                        }

                        printf("%s(", temp);

                        struct group *grp;

                        if ((grp = getgrgid(fileStat.st_gid)) != NULL)
                        {
                            strcpy(temp, grp->gr_name);
                        }
                        else
                        {
                            strcpy(temp, "");
                        }

                        printf("%s)\t%ld\t", temp, fileStat.st_ino);

                        char *temp2 = (char *)malloc(50);
                        int z = 0;
                        temp2[z++] = (S_ISDIR(fileStat.st_mode) ? 'd' : '-');
                        temp2[z++] = ((fileStat.st_mode & S_IWUSR) ? 'w' : '-');
                        temp2[z++] = ((fileStat.st_mode & S_IXUSR) ? 'x' : '-');
                        temp2[z++] = ((fileStat.st_mode & S_IRUSR) ? 'r' : '-');
                        temp2[z++] = ((fileStat.st_mode & S_IRGRP) ? 'r' : '-');
                        temp2[z++] = ((fileStat.st_mode & S_IWGRP) ? 'w' : '-');
                        temp2[z++] = ((fileStat.st_mode & S_IXGRP) ? 'x' : '-');
                        temp2[z++] = ((fileStat.st_mode & S_IROTH) ? 'r' : '-');
                        temp2[z++] = ((fileStat.st_mode & S_IWOTH) ? 'w' : '-');
                        temp2[z++] = ((fileStat.st_mode & S_IXOTH) ? 'x' : '-');
                        temp2[z] = '\0';

                        printf("%s\t", temp2);
                        free(temp2);
                        free(temp);

                        printf("%ld\t%s\n", fileStat.st_size, hierarchy[i].directories[j].sub_dirs[k].name);

                        struct tm time_info = *(gmtime(&fileStat.st_ctime));
                        printf("\t%s %s %d %d:%d:%d %d\t", days[time_info.tm_wday], months[time_info.tm_mon], time_info.tm_mday, time_info.tm_hour, time_info.tm_min, time_info.tm_sec, time_info.tm_year + 1900);

                        time_info = *(gmtime(&fileStat.st_mtime));
                        printf("\t%s %s %d %d:%d:%d %d\n", days[time_info.tm_wday], months[time_info.tm_mon], time_info.tm_mday, time_info.tm_hour, time_info.tm_min, time_info.tm_sec, time_info.tm_year + 1900);
                    }
                    printf("\n");
                }

                if (hierarchy[i].directories[j].num_files > 0)
                {
                    printf("Files\n");
                    for (int k = 0; k < hierarchy[i].directories[j].num_files; k++)
                    {
                        struct stat fileStat;
                        if (stat(hierarchy[i].directories[j].files_in_dir[k].path, &fileStat) < 0)
                            continue;

                        char *temp = (char *)malloc(1000);

                        struct passwd *pwd;
                        if ((pwd = getpwuid(fileStat.st_uid)) != NULL)
                        {
                            strcpy(temp, pwd->pw_name);
                        }
                        else
                        {
                            strcpy(temp, "");
                        }

                        printf("%s(", temp);

                        struct group *grp;

                        if ((grp = getgrgid(fileStat.st_gid)) != NULL)
                        {
                            strcpy(temp, grp->gr_name);
                        }
                        else
                        {
                            strcpy(temp, "");
                        }

                        printf("%s)\t%ld\t", temp, fileStat.st_ino);

                        char *temp2 = (char *)malloc(50);
                        int z = 0;
                        temp2[z++] = (S_ISDIR(fileStat.st_mode) ? 'd' : '-');
                        temp2[z++] = ((fileStat.st_mode & S_IWUSR) ? 'w' : '-');
                        temp2[z++] = ((fileStat.st_mode & S_IXUSR) ? 'x' : '-');
                        temp2[z++] = ((fileStat.st_mode & S_IRUSR) ? 'r' : '-');
                        temp2[z++] = ((fileStat.st_mode & S_IRGRP) ? 'r' : '-');
                        temp2[z++] = ((fileStat.st_mode & S_IWGRP) ? 'w' : '-');
                        temp2[z++] = ((fileStat.st_mode & S_IXGRP) ? 'x' : '-');
                        temp2[z++] = ((fileStat.st_mode & S_IROTH) ? 'r' : '-');
                        temp2[z++] = ((fileStat.st_mode & S_IWOTH) ? 'w' : '-');
                        temp2[z++] = ((fileStat.st_mode & S_IXOTH) ? 'x' : '-');
                        temp2[z] = '\0';

                        printf("%s\t", temp2);
                        
                        free(temp2);
                        free(temp);

                        printf("%ld\t%s\n", fileStat.st_size, hierarchy[i].directories[j].files_in_dir[k].name);

                        struct tm time_info = *(gmtime(&fileStat.st_ctime));
                        printf("\t%s %s %d %d:%d:%d %d\t", days[time_info.tm_wday], months[time_info.tm_mon], time_info.tm_mday, time_info.tm_hour, time_info.tm_min, time_info.tm_sec, time_info.tm_year + 1900);

                        time_info = *(gmtime(&fileStat.st_mtime));
                        printf("\t%s %s %d %d:%d:%d %d\n", days[time_info.tm_wday], months[time_info.tm_mon], time_info.tm_mday, time_info.tm_hour, time_info.tm_min, time_info.tm_sec, time_info.tm_year + 1900);
                    }
                    printf("\n");
                }
            }
        }

        for (int i = 0; i < 100; i++)
        {
            for (int j = 0; j < 200; j++)
            {
                for (int k = 0; k < 200; k++)
                {
                    free(hierarchy[i].directories[j].files_in_dir[k].name);
                    free(hierarchy[i].directories[j].sub_dirs[k].path);
                    free(hierarchy[i].directories[j].files_in_dir[k].path);
                    free(hierarchy[i].directories[j].sub_dirs[k].name);
                }
                free(hierarchy[i].directories[j].files_in_dir);
                free(hierarchy[i].directories[j].sub_dirs);
                free(hierarchy[i].directories[j].name);
            }
            free(hierarchy[i].directories);
        }
        free(hierarchy);
    }
    else if (strcmp(method, "-inode") == 0)
    {
        /* TODO */

        Ihierarchy = (inode_dir_lev *)malloc(100 * sizeof(inode_dir_lev));
        for (int i = 0; i < 100; i++)
        {
            Ihierarchy[i].num_dirs = 0;
            Ihierarchy[i].directories = (inode_directory *)malloc(200 * sizeof(inode_directory));

            for (int j = 0; j < 200; j++)
            {
                Ihierarchy[i].directories[j].num_files = 0;
                Ihierarchy[i].directories[j].num_subdirs = 0;

                Ihierarchy[i].directories[j].name = (char *)malloc(500 * sizeof(char));
                strcpy(Ihierarchy[i].directories[j].name, "");

                Ihierarchy[i].directories[j].files_in_dir = (inode_file_info *)malloc(200 * sizeof(inode_file_info));
                Ihierarchy[i].directories[j].sub_dirs = (inode_file_info *)malloc(200 * sizeof(inode_file_info));

                for (int k = 0; k < 200; k++)
                {
                    Ihierarchy[i].directories[j].files_in_dir[k].size = 0;
                    Ihierarchy[i].directories[j].files_in_dir[k].inode_num = 0;
                    Ihierarchy[i].directories[j].files_in_dir[k].blocks = 0;
                    Ihierarchy[i].directories[j].files_in_dir[k].division = 0;

                    Ihierarchy[i].directories[j].files_in_dir[k].name = (char *)malloc(200 * sizeof(char));
                    strcpy(Ihierarchy[i].directories[j].files_in_dir[k].name, "");
                }

                for (int k = 0; k < 200; k++)
                {
                    Ihierarchy[i].directories[j].sub_dirs[k].size = 0;
                    Ihierarchy[i].directories[j].sub_dirs[k].inode_num = 0;
                    Ihierarchy[i].directories[j].sub_dirs[k].blocks = 0;
                    Ihierarchy[i].directories[j].sub_dirs[k].division = 0;

                    Ihierarchy[i].directories[j].sub_dirs[k].name = (char *)malloc(200 * sizeof(char));
                    strcpy(Ihierarchy[i].directories[j].sub_dirs[k].name, "");
                }
            }
        }

        strcpy(Ihierarchy[0].directories[0].name, path);
        Ihierarchy[0].num_dirs = 1;

        int level = 1;

        listINODEinfo(path, path, level);

        for (int i = 0; i < highest_level; i++)
        {
            for (int j = 0; j < Ihierarchy[i].num_dirs; j++)
            {
                inode_file_info *combined = (inode_file_info *)malloc(1000 * sizeof(inode_file_info));
                for (int k = 0; k < 1000; k++)
                {
                    combined[k].name = (char *)malloc(500);
                }

                int itr = 0;

                for (int k = 0; k < Ihierarchy[i].directories[j].num_files; k++)
                {
                    // printf("Name : %s Size : %d\n", Ihierarchy[i].directories[j].files_in_dir[k].name, Ihierarchy[i].directories[j].files_in_dir[k].size);
                    strcpy(combined[itr].name, Ihierarchy[i].directories[j].files_in_dir[k].name);
                    combined[itr].blocks = Ihierarchy[i].directories[j].files_in_dir[k].blocks;
                    combined[itr].size = Ihierarchy[i].directories[j].files_in_dir[k].size;
                    combined[itr].division = Ihierarchy[i].directories[j].files_in_dir[k].division;
                    combined[itr++].inode_num = Ihierarchy[i].directories[j].files_in_dir[k].inode_num;
                }

                for (int k = 0; k < Ihierarchy[i].directories[j].num_subdirs; k++)
                {
                    // printf("Name : %s Size : %d\n", Ihierarchy[i].directories[j].sub_dirs[k].name, Ihierarchy[i].directories[j].sub_dirs[k].size);

                    strcpy(combined[itr].name, Ihierarchy[i].directories[j].sub_dirs[k].name);
                    combined[itr].blocks = Ihierarchy[i].directories[j].sub_dirs[k].blocks;
                    combined[itr].size = Ihierarchy[i].directories[j].sub_dirs[k].size;
                    combined[itr].division = Ihierarchy[i].directories[j].sub_dirs[k].division;
                    combined[itr++].inode_num = Ihierarchy[i].directories[j].sub_dirs[k].inode_num;
                }

                merge_sort_inode(combined, itr, 0, itr - 1);

                if (itr != 0)
                {
                    printf("Level %d Inodes: %s\n", (i + 1), Ihierarchy[i].directories[j].name);

                    for (int k = 0; k < itr; k++)
                    {
                        printf("%d:%d\t%d\t%d\t%s\n", combined[k].inode_num, combined[k].size, combined[k].blocks, combined[k].division, combined[k].name);
                    }
                    printf("\n");
                }

                for (int k = 0; k < 1000; k++)
                {
                    free(combined[k].name);
                }
                free(combined);
            }
        }
        for (int i = 0; i < 100; i++)
        {
            for (int j = 0; j < 200; j++)
            {
                for (int k = 0; k < 200; k++)
                {
                    free(Ihierarchy[i].directories[j].sub_dirs[k].name);
                    free(Ihierarchy[i].directories[j].files_in_dir[k].name);
                }
                free(Ihierarchy[i].directories[j].name);
                free(Ihierarchy[i].directories[j].files_in_dir);
                free(Ihierarchy[i].directories[j].sub_dirs);
            }
            free(Ihierarchy[i].directories);
        }
        free(Ihierarchy);
    }
    else
    {
        printf("Invalid Method\n");
    }

    return 0;
}
