#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#
IDF_PATH=/opt/Espressif/esp-idf/
PROJECT_NAME := esp32-simple-webserver

include $(IDF_PATH)/make/project.mk

COMPONENT_ADD_INCLUDEDIRS := components
