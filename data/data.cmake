install (FILES ${CMAKE_SOURCE_DIR}/data/org.ukui.samba.share.config.conf DESTINATION /usr/share/dbus-1/system.d/)
install (FILES ${CMAKE_SOURCE_DIR}/data/org.ukui.samba.share.config.policy DESTINATION /usr/share/polkit-1/actions)
install (FILES ${CMAKE_SOURCE_DIR}/data/org.ukui.samba.share.config.service DESTINATION /usr/share/dbus-1/system-services)
