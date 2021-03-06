SET(PREFIX_ext_pyqt "${EXTPREFIX}" )
if (UNIX)
    ExternalProject_Add( ext_pyqt
        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        URL http://files.kde.org/krita/build/dependencies/PyQt5_gpl-5.6.tar.gz
        URL_MD5 dbfc885c0548e024ba5260c4f44e0481
        
        CONFIGURE_COMMAND ${PREFIX_ext_pyqt}/bin/python3 <SOURCE_DIR>/configure.py --confirm-license
        BUILD_COMMAND make
        INSTALL_COMMAND make install
        
        BUILD_IN_SOURCE 1

        UPDATE_COMMAND ""
        ALWAYS 0
    )
elseif(MINGW)
    ExternalProject_Add( ext_pyqt
        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
        URL http://files.kde.org/krita/build/dependencies/PyQt5_gpl-5.6.zip
        URL_MD5 916e79bacda1799f8db7bc034043e450
        
        CONFIGURE_COMMAND python.exe <SOURCE_DIR>/configure.py --confirm-license --target-py-version 3.6 --bindir {PREFIX_ext_pyqt}/bin --qt ${PREFIX_ext_pyqt} --sip ${PREFIX_ext_pyqt}/bin/sip.exe --sip-incdir ${PREFIX_ext_pyqt}/include --target-py-version 3.6 --spec win32-g++ --verbose --sipdir ${PREFIX_ext_pyqt}/share/sip --destdir ${PREFIX_ext_pyqt}/bin --no-qml-plugin --no-python-dbus --no-qsci-api --no-tools
        BUILD_COMMAND mingw32-make
        INSTALL_COMMAND mingw32-make install
        
        BUILD_IN_SOURCE 1

        UPDATE_COMMAND ""
        ALWAYS 0
    )

endif()

