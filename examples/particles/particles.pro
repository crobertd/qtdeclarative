TEMPLATE = subdirs
SUBDIRS += affectors \
    customparticle \
    emitters \
    imageparticle \
    system

#Install shared images too
qml.files = images
qml.path = $$[QT_INSTALL_EXAMPLES]/qtquick/particles
INSTALLS = qml
