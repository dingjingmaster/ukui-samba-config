install (FILES ${CMAKE_SOURCE_DIR}/data/org.ukui.samba.config.conf DESTINATION /usr/share/dbus-1/system.d/)
install (FILES ${CMAKE_SOURCE_DIR}/data/org.ukui.samba.config.policy DESTINATION /usr/share/polkit-1/actions)
install (FILES ${CMAKE_SOURCE_DIR}/data/org-ukui-samba-config.service DESTINATION /usr/share/dbus-1/system-services)


#
#[D-BUS Service]
#Name=org.freedesktop.UDisks2
#Exec=/usr/lib/udisks2/udisksd
#User=root
#SystemdService=udisks2.service
