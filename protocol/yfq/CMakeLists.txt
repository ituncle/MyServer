aux_source_directory(. SRC_FILES)
set(INC_FILES errcode.h
        yfqHandler.h
        yfqParser.h
        yfqprotoserver3.pb.h)

add_library(protocol_yfq STATIC ${SRC_FILES} ${INC_FILES})
target_link_libraries(protocol_yfq muduo_base_cpp11 muduo_net_cpp11 utils)
#set_target_properties(protocol PROPERTIES COMPILE_FLAGS -fPIC)