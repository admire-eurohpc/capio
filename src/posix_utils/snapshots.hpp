#ifndef CAPIO_SNAPSHOTS_HPP
#define CAPIO_SNAPSHOTS_HPP

int* get_fd_snapshot(long tid) {

    std::string shm_name = "capio_snapshot_" + std::to_string(tid);

    int* fd_shm = (int*) get_shm_if_exist(shm_name);
    return fd_shm;
}


void initialize_from_snapshot(int* fd_shm,CPFiles_t * files,CPFileDescriptors_t * capio_files_descriptors,
                              CPFilesPaths_t * capio_files_paths, long tid) {
    int i = 0;
    std::string shm_name;
    int fd;
    off64_t* p_shm;
    char* path_shm;

    CAPIO_DBG("initialize_from_snapshot \n");


    std::string pid = std::to_string(tid);

    while ((fd = fd_shm[i]) != -1) {
        shm_name = "capio_snapshot_path_" + pid + "_" + std::to_string(fd);
        path_shm = (char*) get_shm(shm_name);
        (*capio_files_descriptors)[fd] = path_shm;
        capio_files_paths->insert(path_shm);
        if (munmap(path_shm, PATH_MAX) == -1) {
            err_exit("munmap initialize_from_snapshot", "initialize_from_snapshot");
        }
        if (shm_unlink(shm_name.c_str()) == -1) {
            err_exit("shm_unlink snapshot " + shm_name, "initialize_from_snapshot");
        }
        shm_name = "capio_snapshot_" + pid + "_" + std::to_string(fd);
        p_shm = (off64_t*) get_shm(shm_name);
        std::string shm_name_offset = "offset_" + pid + "_" + std::to_string(fd);
        std::get<0>((*files)[fd]) = (off64_t*) create_shm(shm_name_offset, sizeof(off64_t));
        *std::get<0>((*files)[fd]) = p_shm[1];
        std::get<1>((*files)[fd]) = new off64_t;
        *std::get<1>((*files)[fd]) = p_shm[2];
        std::get<2>((*files)[fd]) = p_shm[3];
        std::get<3>((*files)[fd]) = p_shm[4];
        if (munmap(p_shm, 6 * sizeof(off64_t)) == -1) {
            err_exit("munmap 2 initialize_from_snapshot", "initialize_from_snapshot");
        }
        if (shm_unlink(shm_name.c_str()) == -1) {
            err_exit("shm_unlink snapshot " + shm_name, "initialize_from_snapshot");
        }
        ++i;
    }
    shm_name = "capio_snapshot_" + pid;
    if (shm_unlink(shm_name.c_str()) == -1) {
        err_exit("shm_unlink snapshot " + shm_name, "initialize_from_snapshot");
    }

    CAPIO_DBG("initialize_from_snapshot ending\n");

}


//* fd -> (shm*, *offset, *mapped_shm_size, *offset_upper_bound, file status flags, file_descriptor_flags)
//std::unordered_map<int, std::tuple<void*, off64_t*, off64_t*, off64_t*, int, int>>* files = nullptr;
//std::unordered_map<int, std::string>* capio_files_descriptors = nullptr;
//std::unordered_set<std::string>* capio_files_paths = nullptr;
void create_snapshot(CPFiles_t* files, CPFileDescriptors_t* capio_files_descriptors, long tid) {
    int fd, status_flags, fd_flags;
    off64_t offset, mapped_shm_size;
    off64_t* p_shm;
    int* fd_shm;
    char* path_shm;

    std::string pid = std::to_string(tid);

    int n_fd = files->size();
    if (n_fd == 0)
        return;
    fd_shm = (int*)create_shm("capio_snapshot_" + pid, (n_fd + 1) * sizeof(int));

    CAPIO_DBG("creating snapshots\n");

    int i = 0;
    for (auto& p : *files) {
        fd = p.first;
        p_shm = (off64_t*) create_shm("capio_snapshot_" + pid + "_" + std::to_string(fd), 5 * sizeof(off64_t));

        CAPIO_DBG("creating snapshot fd%d\n", fd);

        fd_shm[i] = fd;
        offset = *std::get<0>(p.second);
        mapped_shm_size = *std::get<1>(p.second);
        status_flags = std::get<2>(p.second);
        fd_flags = std::get<3>(p.second);
        p_shm[0] = fd;
        p_shm[1] = offset;
        p_shm[2] = mapped_shm_size;
        p_shm[3] = status_flags;
        p_shm[4] = fd_flags;
        std::string shm_name = "capio_snapshot_path_" + pid + "_" + std::to_string(fd);

        CAPIO_DBG("snapshot path name %s\n", shm_name.c_str());

        path_shm = (char*) create_shm(shm_name.c_str(), PATH_MAX * sizeof(char));
        strcpy(path_shm, (*capio_files_descriptors)[fd].c_str());
        ++i;
    }
    fd_shm[i] = -1;
}
#endif //CAPIO_SNAPSHOTS_HPP