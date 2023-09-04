#ifndef CAPIO_SERVER_DATA_TYPES_HPP
#define CAPIO_SERVER_DATA_TYPES_HPP

typedef std::vector<std::tuple<int, FILE*, bool>> CSFDFileLocationReadsVector_t;
typedef std::unordered_map<int, int> CSPidsMap_T;
typedef std::unordered_map<int, std::string> CSAppsMap_t;
typedef std::unordered_map<std::string, std::unordered_set<std::string>> CSFilesSentMap_t;
typedef std::unordered_map<int, std::unordered_map<int, std::tuple<Capio_file*, off64_t*>>> CSProcessFileMap_t;
typedef std::unordered_map<int, std::unordered_map<int, std::string>> CSProcessFileMetadataMap_t;
typedef std::unordered_map<int, Circular_buffer<off_t>*> CSResponseBufferMap_t;
typedef std::unordered_map<int, std::pair<SPSC_queue<char>*, SPSC_queue<char>*>> CSDataBufferMap_t;
typedef std::unordered_map<std::string, Capio_file*> CSFilesMetadata_t;
typedef std::unordered_map<std::string, std::tuple<std::string, std::string, std::string, long int, bool, long int>> CSMetadataConfMap_t;
typedef std::vector<std::tuple<std::string, std::string, std::string, std::string, long int, long int, bool, long int>> CSMetadataConfGlobs_t;
typedef std::unordered_map<int, std::unordered_map<std::string, bool>> CSWritersMap_t;
typedef std::unordered_map<std::string, std::pair<char*, long int>> CSFilesLocationMap_t;
typedef std::unordered_map<std::string, int> CSNodesHelperRankMap_t;
typedef std::unordered_map<int, std::string> CSRankToNodeMap_t;
typedef std::unordered_map<std::string, std::vector<std::tuple<int, int, off64_t, bool>>> CSPendingReadsMap_t;
typedef std::unordered_map<std::string, std::list<std::tuple<int, int, off64_t, bool>>> CSMyRemotePendingReads_t;
typedef std::unordered_map<std::string, std::list<int>> CSMyRemotePendingStats_t;
typedef std::unordered_map<std::string, std::list<std::tuple<size_t, size_t, sem_t*>>> CSClientsRemotePendingReads_t;
typedef std::unordered_map<std::string, std::list<sem_t*>> CSClientsRemotePendingStats_t;
typedef std::unordered_set<std::string> CSOnDiskMap_t;
typedef std::unordered_map<std::string, std::list<struct remote_n_files*>> CSClientsRemotePendingNFilesMap_t;
typedef std::unordered_map<int, sem_t*> CSSemsWriteMap_t;
typedef Circular_buffer<char> CSCircularBuff_t;



#endif //CAPIO_DATA_TYPES_HPP
