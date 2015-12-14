//
// Created by sh on 29.11.2015.
//

#ifndef OSGOCULUSPROTO_VXOCULUSVIEWER_H
#define OSGOCULUSPROTO_VXOCULUSVIEWER_H

#include <osgViewer/viewer>
#include <osg/ref_ptr>
#include <osg/MatrixTransform>
#include <osg/Texture2D>
#include <osg/BlendFunc>
#include <osg/AlphaFunc>
#include <osgDB/WriteFile>

class VXOculusViewer : public osgViewer::Viewer {

public:
    VXOculusViewer();
    virtual ~VXOculusViewer();

    int run();



private:
    osgViewer::Viewer viewer;
    osg::ref_ptr<osg::Group> sceneGraphRoot;
    osg::ref_ptr<osg::Node> sceneRoot;

    void buildSceneGraph();
    osg::ref_ptr<osg::Group> createTexturePlane();
    osg::ref_ptr<osg::Node> createExampleScene();
    osg::ref_ptr<osg::MatrixTransform> mt;

    osg::ref_ptr<osg::Image> imageLeft;
    osg::ref_ptr<osg::Image> imageRight;
};


#endif //OSGOCULUSPROTO_VXOCULUSVIEWER_H
