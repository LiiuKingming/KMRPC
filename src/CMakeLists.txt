aux_source_directory(. SRC_LIST)

set(SRC_LIST
    kmrpcapplication.cc
    kmrpcconfig.cc 
    kmrpcprovider.cc
    kmrpcchannel.cc 
    kmrpccontroller.cc
    rpcheader.pb.cc 
    logger.cc
    zookeeperutil.cc )

# 生成静态库, 与muduo链接
add_library(kmrpc ${SRC_LIST})
# 设置库依赖, 注意顺序
target_link_libraries(kmrpc muduo_net muduo_base pthread zookeeper_mt)
