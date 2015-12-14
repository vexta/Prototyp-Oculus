#include <iostream>
#include <sstream>

#include <osgViewer/viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osg/MatrixTransform>
#include <osg/ShadeModel>
#include <osg/CullFace>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osg/Point>
#include <osg/Material>
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <osg/BlendFunc>
#include <osg/AlphaFunc>

#include <osgGA/TrackballManipulator>

#include "VXOculusViewer.h"



osg::Node* example1_createGeodes() {
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    vertices->push_back( osg::Vec3(-1.f,  1.f, 0.f));
    vertices->push_back( osg::Vec3(-1.f, -1.f, 0.f));
    vertices->push_back( osg::Vec3( 1.f, -1.f, 0.f));
    vertices->push_back( osg::Vec3( 1.f,  1.f, 0.f));

    osg::ref_ptr<osg::Vec2Array> texture_coordinates = new osg::Vec2Array;
    texture_coordinates->push_back(osg::Vec2(0.f, 0.f));
    texture_coordinates->push_back(osg::Vec2(0.f, 1.f));
    texture_coordinates->push_back(osg::Vec2(1.f, 1.f));
    texture_coordinates->push_back(osg::Vec2(1.f, 0.f));

    osg::ref_ptr<osg::Vec4Array> colours = new osg::Vec4Array;
    colours->push_back( osg::Vec4( 1.f, 1.f, 1.f, 1.f ) );

    osg::ref_ptr<osg::Group> grp = new osg::Group;
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    geom->setVertexArray(vertices.get());
    geom->setTexCoordArray(0, texture_coordinates.get());
    geom->setColorArray(colours.get());
    geom->setColorBinding(osg::Geometry::BIND_OVERALL);
    geom->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(geom.get());
    grp->addChild(geode.get());
    return grp.release();
}

osg::ref_ptr<osg::Node> example2_createTextureSceneGraph () {
    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec4Array> colours = new osg::Vec4Array;
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;

    geometry->setVertexArray(vertices.get());
    // x+ = right, y+ = forward, z+ = up; relative to user
    vertices->push_back(osg::Vec3(-.5f, -.5f, 0.f));
    vertices->push_back(osg::Vec3( .5f, -.5f, 0.f));
    vertices->push_back(osg::Vec3( .5f,  .5f, 0.f));
    vertices->push_back(osg::Vec3(-.5f,  .5f, 0.f));

    geometry->setColorArray(colours.get());
    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    // RGBA
    colours->push_back(osg::Vec4(1.f, 0.f, 0.f, 1.f));
    colours->push_back(osg::Vec4(0.f, 1.f, 0.f, 1.f));
    colours->push_back(osg::Vec4(0.f, 0.f, 1.f, 1.f));
    colours->push_back(osg::Vec4(1.f, 1.f, 1.f, 1.f));

    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
    geode->addDrawable(geometry.get());
    return geode.get();
}

osg::ref_ptr<osg::Node> example1_createSceneGraph() {
    osg::ref_ptr<osg::Node> node =  example1_createGeodes();
    osg::StateSet* state = node->getOrCreateStateSet();
    std::string fileName("container.jpg");
    osg::ref_ptr<osg::Image> image;
    osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D;

    state->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
    state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

    image = osgDB::readImageFile(fileName);
    if (!image.valid())  {
        osg::notify( osg::FATAL ) << "Unable to load data file. Exiting." << std::endl;
        return(NULL);
    }

    // Set the image in a Texture2D object
    tex->setImage(image.get());
    // After creating the OpenGL texture object, release the
    // internal ref_ptr<Image> (to delete the Image).
    tex->setUnRefImageDataAfterApply(true);
    state->setTextureAttributeAndModes(0, tex.get());

    // Turn on blending
    osg::BlendFunc* bf = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
    state->setAttributeAndModes(bf);

    // Turn on alpha testing
    osg::AlphaFunc* af = new osg::AlphaFunc(osg::AlphaFunc::GREATER, 0.05f);
    state->setAttributeAndModes(af);

    return node.get();
}

osg::ref_ptr<osg::Node> example2_createSceneGraph() {
    osg::ref_ptr<osg::Node> node =  example1_createGeodes();
    osg::ref_ptr<osg::Group> sceneGraph = new osg::Group;
    osg::StateSet* state = node->getOrCreateStateSet();

    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    osg::ref_ptr<osg::Image> image = new osg::Image;
    osg::ref_ptr<osg::Texture2D> texture2D = new osg::Texture2D;
    osg::Matrix view;
    view.makeTranslate(0.3, 0.0, 0.0);

    state->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
    state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

    camera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
    camera->setRenderOrder(osg::Camera::PRE_RENDER);
    camera->setProjectionMatrixAsOrtho2D(-1.0, 1.0, -1.0, 1.0);
    camera->setViewport(0, 0, 1024, 1024);
    camera->setViewMatrix(view);
    camera->setClearColor(osg::Vec4(0.1f, 0.3f, 0.0f, 1.0f));
    camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    image->allocateImage(1024, 1024, 1, GL_RGBA, GL_UNSIGNED_BYTE);
    image->setInternalTextureFormat(GL_RGBA8);

    texture2D->setImage(image);
    camera->attach(osg::Camera::COLOR_BUFFER, texture2D.get());
    camera->addChild(example2_createTextureSceneGraph());
    // After creating the OpenGL texture object, release the
    // internal ref_ptr<Image> (to delete the Image).

    texture2D->setUnRefImageDataAfterApply(true);
    state->setTextureAttributeAndModes(0, texture2D.get(), osg::StateAttribute::ON);

    // Turn on blending
    osg::BlendFunc* bf = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
    state->setAttributeAndModes(bf);

    // Turn on alpha testing
    osg::AlphaFunc* af = new osg::AlphaFunc(osg::AlphaFunc::GREATER, 0.05f);
    state->setAttributeAndModes(af);

    sceneGraph->addChild(camera);
    sceneGraph->addChild(node);
    return sceneGraph.get();
}

// texture maping
int example1_main() {
    osgViewer::Viewer v;
    v.setSceneData(example1_createSceneGraph());
    return v.run();
}

// render to texture
int example2_main() {
    osgViewer::Viewer v;
    v.setSceneData(example2_createSceneGraph());
    return v.run();
    /*v.getCamera()->setProjectionMatrixAsPerspective(40., 1., 1., 100.);
    osg::Matrix translate;
    translate.makeTranslate(-.5, 0.25, -4.5);
    while (!v.done()) {
        osg::Matrix rotate;
        v.getCamera()->setViewMatrix(translate);
        v.frame();
    }
    return 0;*/
}

int example3_main() {
    VXOculusViewer v;
    return v.run();
}


int example4_main() {
    osgViewer::Viewer v;
    v.setCameraManipulator(new osgGA::TrackballManipulator);
    v.addEventHandler(new osgViewer::ThreadingHandler);
    v.setThreadingModel(osgViewer::Viewer::SingleThreaded);
    v.addEventHandler(new osgViewer::StatsHandler);

    v.setSceneData(osgDB::readNodeFile("cow.osg"));
    v.setSceneData(example2_createTextureSceneGraph());

    int width = 1024;
    int height = 768;

    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->x = 0;
    traits->y = 0;
    traits->width = 1024;
    traits->height = 768;
    traits->windowDecoration = true;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;
    traits->vsync = false;
    osg::ref_ptr<osg::GraphicsContext> graphicsContext = osg::GraphicsContext::createGraphicsContext(traits);

    osg::ref_ptr<osg::Camera> cam = new osg::Camera;
    osg::ref_ptr<osg::Texture2D> camTexture = new osg::Texture2D;
    cam->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    cam->setRenderOrder(osg::Camera::PRE_RENDER);
    cam->setName("Render Color");
    camTexture->setSourceFormat(GL_RGBA);
    camTexture->setInternalFormat(GL_RGBA32F_ARB);
    camTexture->setSourceType(GL_FLOAT);
    cam->attach(osg::Camera::COLOR_BUFFER0, camTexture);
    cam->setViewport(0, 0, width, height);
    cam->setGraphicsContext(graphicsContext);
    v.addSlave(cam);

    osg::ref_ptr<osg::Camera> disp = new osg::Camera;
    disp->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    disp->setRenderOrder(osg::Camera::POST_RENDER);
    disp->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    disp->setViewMatrix(osg::Matrix());
    disp->setProjectionMatrix(osg::Matrix::ortho2D(0, 1, 0, 1));
    disp->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    osg::Geometry* geom = osg::createTexturedQuadGeometry(osg::Vec3(), osg::Vec3(1.0f, 0.0f, 0.0f),
      osg::Vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f, 1.0f, 1.0f);
    osg::Geode *quad = new osg::Geode;
    quad->addDrawable(geom);
    disp->addChild(quad);
    disp->setName("Display");

    disp->setGraphicsContext(graphicsContext);
    disp->setViewport(0, 0, width, height);
    disp->getOrCreateStateSet()->setTextureAttributeAndModes(0, camTexture.get(), osg::StateAttribute::ON);

    v.addSlave(disp, osg::Matrix(), osg::Matrix(), false);

    v.realize();

    while (!v.done()) {
        v.frame();
    }
    return 0;
}



int main(int argc, char *argv[]) {
    int example;
    std::istringstream(std::string(argv[1])) >> example;
    std::cout << "Starting example " << example << std::endl;

    switch (example) {
        case 1:
            return example1_main();
        case 2:
            return example2_main();
        case 3:
            return example3_main();
        case 4:
            return example4_main();
        default:
            std::cout << "Example " << example << " not found" << std::endl;
    }
    return 1;
}