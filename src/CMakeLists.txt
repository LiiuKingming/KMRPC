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

# ���ɾ�̬��, ��muduo����
add_library(kmrpc ${SRC_LIST})
# ���ÿ�����, ע��˳��
target_link_libraries(kmrpc muduo_net muduo_base pthread zookeeper_mt)
