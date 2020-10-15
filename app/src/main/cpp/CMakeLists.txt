cmake_minimum_required(VERSION 3.4.1)

# 设置生成的so动态库最后输出的路径
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})

# 支持C++11标准和GNU扩展特性
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

# 1. 定义so库和头文件所在目录，方面后面使用
set(ffmpeg_lib_dir ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})
set(ffmpeg_head_dir ${CMAKE_SOURCE_DIR}/ffmpeg)

# 2. 添加头文件目录
include_directories(${ffmpeg_head_dir}/include)

# 3. 添加ffmpeg相关的so库
add_library( avutil
        SHARED
        IMPORTED )
set_target_properties( avutil
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavutil.so )

add_library( swresample
        SHARED
        IMPORTED )
set_target_properties( swresample
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libswresample.so )

add_library( avcodec
        SHARED
        IMPORTED )
set_target_properties( avcodec
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavcodec.so )

add_library( avfilter
        SHARED
        IMPORTED)
set_target_properties( avfilter
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavfilter.so )

add_library( swscale
        SHARED
        IMPORTED)
set_target_properties( swscale
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libswscale.so )

add_library( avformat
        SHARED
        IMPORTED)
set_target_properties( avformat
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavformat.so )

add_library( avdevice
        SHARED
        IMPORTED)
set_target_properties( avdevice
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavdevice.so )

add_library( h264
        SHARED
        IMPORTED)
set_target_properties( h264
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libh264.so )

include_directories(${ffmpeg_head_dir}/include)

# 查找代码中使用到的系统库
find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log )

# 配置目标so库编译信息
add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        native-lib.cpp

        # 工具
        ${CMAKE_SOURCE_DIR}/utils/logger.h
        ${CMAKE_SOURCE_DIR}/utils/timer.c

        # 播放器
        ${CMAKE_SOURCE_DIR}/media/player/def_player/player.cpp
        ${CMAKE_SOURCE_DIR}/media/player/gl_player/gl_player.cpp

        # 解码器
        ${CMAKE_SOURCE_DIR}/media/one_frame.h
        ${CMAKE_SOURCE_DIR}/media/decoder/i_decoder.h
        ${CMAKE_SOURCE_DIR}/media/decoder/decode_state.h
        ${CMAKE_SOURCE_DIR}/media/decoder/i_decode_state_cb.cpp
        ${CMAKE_SOURCE_DIR}/media/decoder/base_decoder.cpp
        ${CMAKE_SOURCE_DIR}/media/decoder/video/v_decoder.cpp
        ${CMAKE_SOURCE_DIR}/media/decoder/audio/a_decoder.cpp

        # 编码器
        ${CMAKE_SOURCE_DIR}/media/encoder/i_encoder.h
        ${CMAKE_SOURCE_DIR}/media/encoder/base_encoder.cpp
        ${CMAKE_SOURCE_DIR}/media/encoder/i_encode_state_cb.h
        ${CMAKE_SOURCE_DIR}/media/encoder/video/v_encoder.cpp
        ${CMAKE_SOURCE_DIR}/media/encoder/audio/a_encoder.cpp

        ${CMAKE_SOURCE_DIR}/media/const.h


        # 打包器
        ${CMAKE_SOURCE_DIR}/media/muxer/ff_repack.cpp

        ${CMAKE_SOURCE_DIR}/media/muxer/mp4_muxer.cpp
        ${CMAKE_SOURCE_DIR}/media/muxer/i_muxer_cb.h

        # 渲染器
        ${CMAKE_SOURCE_DIR}/media/render/video/video_render.h
        ${CMAKE_SOURCE_DIR}/media/render/video/native_render/native_render.cpp
        ${CMAKE_SOURCE_DIR}/media/render/video/opengl_render/opengl_render.cpp

        ${CMAKE_SOURCE_DIR}/media/render/audio/audio_render.h
        ${CMAKE_SOURCE_DIR}/media/render/audio/opensl_render.cpp
        ${CMAKE_SOURCE_DIR}/media/render/video/opengl_render/opengl_pixel_receiver.h

        # OpenGL EGL核心
        ${CMAKE_SOURCE_DIR}/opengl/egl/egl_core.cpp
        ${CMAKE_SOURCE_DIR}/opengl/egl/egl_surface.cpp

        # OpenGL绘制器
        ${CMAKE_SOURCE_DIR}/opengl/drawer/drawer.cpp
        ${CMAKE_SOURCE_DIR}/opengl/drawer/video_drawer.cpp

        # OpenGL绘制代理器
        ${CMAKE_SOURCE_DIR}/opengl/drawer/proxy/drawer_proxy.cpp
        ${CMAKE_SOURCE_DIR}/opengl/drawer/proxy/def_drawer_proxy_impl.cpp

        #合成器
        ${CMAKE_SOURCE_DIR}/media/synthesizer/synthesizer.cpp
)

# 指定编译目标库时，cmake要链接的库
target_link_libraries(

        # 指定目标库，native-lib 是在上面 add_library 中配置的目标库
        native-lib

        # 4. 连接 FFmpeg 相关的库
        avutil
        swresample
        avcodec
        avfilter
        swscale
        avformat
        avdevice
        h264

        -landroid
        OpenSLES
        -lEGL
        -lGLESv2
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib} )