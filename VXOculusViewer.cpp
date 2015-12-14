//
// Created by sh on 29.11.2015.
//

#include "VXOculusViewer.h"

VXOculusViewer::VXOculusViewer() {

}

VXOculusViewer::~VXOculusViewer() {

}

int VXOculusViewer::run() {
    buildSceneGraph();
    viewer.setSceneData(sceneGraphRoot);
    viewer.getCamera()->setProjectionMatrixAsPerspective(45., 0.3, 0.3, 100.);
    viewer.getCamera()->setClearColor(osg::Vec4(0.0, 0.3, 0.0, 1.0));
    osg::Matrix translate;
    translate.makeTranslate(-0.73, 0.5, -5.0);
    viewer.getCamera()->setViewMatrix(translate);
    double angle(0.01);
    while(!viewer.done()) {
        osg::Matrix m;
        m.makeRotate(angle, osg::Vec3(1.0, 0.5, 0.0));
        mt->setMatrix(m);
        angle += 0.01;
        viewer.frame();
    }
    return 0;
}

void VXOculusViewer::buildSceneGraph() {
    sceneGraphRoot = new osg::Group;
    sceneRoot = createExampleScene();   // TODO

    osg::ref_ptr<osg::Camera> eyeLeft = new osg::Camera;
    osg::ref_ptr<osg::Camera> eyeRight = new osg::Camera;
    osg::ref_ptr<osg::MatrixTransform> planeLeft = new osg::MatrixTransform;
    osg::ref_ptr<osg::MatrixTransform> planeRight = new osg::MatrixTransform;
    osg::ref_ptr<osg::Group> texPlaneLeft = createTexturePlane();
    osg::ref_ptr<osg::Group> texPlaneRight = createTexturePlane();
    osg::StateSet *stateLeft = texPlaneLeft->getOrCreateStateSet();
    osg::StateSet *stateRight = texPlaneRight->getOrCreateStateSet();
    osg::Matrix m, viewLeft, viewRight;
    imageLeft = new osg::Image;
    imageRight = new osg::Image;
    osg::ref_ptr<osg::Texture2D> textureLeft = new osg::Texture2D;
    osg::ref_ptr<osg::Texture2D> textureRight = new osg::Texture2D;

    stateLeft->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
    stateRight->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
    stateLeft->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    stateRight->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

    viewLeft.makeTranslate(0.1, 0.0, 0.0);
    eyeLeft->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
    eyeLeft->setRenderOrder(osg::Camera::PRE_RENDER);
    eyeLeft->setProjectionMatrixAsOrtho2D(-1.0, 1.0, -1.0, 1.0);
    eyeLeft->setViewport(0, 0, 1024, 1024);
    eyeLeft->setViewMatrix(viewLeft);
    eyeLeft->setClearColor(osg::Vec4(0.0f, 0.0f, 0.3f, 1.0f));
    eyeLeft->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    viewRight.makeTranslate(-0.1, 0.0, 0.0);
    eyeRight->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
    eyeRight->setRenderOrder(osg::Camera::PRE_RENDER);
    eyeRight->setProjectionMatrixAsOrtho2D(-1.0, 1.0, -1.0, 1.0);
    eyeRight->setViewport(0, 0, 1024, 1024);
    eyeRight->setViewMatrix(viewRight);
    eyeRight->setClearColor(osg::Vec4(0.3f, 0.0f, 0.0f, 1.0f));
    eyeRight->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    imageLeft->allocateImage(960, 1080, 1, GL_RGBA, GL_UNSIGNED_BYTE);
    imageLeft->setInternalTextureFormat(GL_RGBA8);
    imageRight->allocateImage(960, 1080, 1, GL_RGBA, GL_UNSIGNED_BYTE);
    imageRight->setInternalTextureFormat(GL_RGBA8);

    textureLeft->setImage(imageLeft);
    eyeLeft->attach(osg::Camera::COLOR_BUFFER, textureLeft.get());
    eyeLeft->addChild(sceneRoot);
    textureRight->setImage(imageRight);
    eyeRight->attach(osg::Camera::COLOR_BUFFER, textureRight.get());
    eyeRight->addChild(sceneRoot);

    textureLeft->setUnRefImageDataAfterApply(true);
    stateLeft->setTextureAttributeAndModes(0, textureLeft.get(), osg::StateAttribute::ON);
    textureRight->setUnRefImageDataAfterApply(true);
    stateRight->setTextureAttributeAndModes(0, textureRight.get(), osg::StateAttribute::ON);

    // Turn on blending
    osg::BlendFunc* bf = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
    stateLeft->setAttributeAndModes(bf);
    stateRight->setAttributeAndModes(bf);

    // Turn on alpha testing
    osg::AlphaFunc* af = new osg::AlphaFunc(osg::AlphaFunc::GREATER, 0.05f);
    stateLeft->setAttributeAndModes(af);
    stateRight->setAttributeAndModes(af);

    sceneGraphRoot->addChild(eyeLeft);
    sceneGraphRoot->addChild(eyeRight);

    m.makeTranslate(-1.0, 0.0, 0.0);
    planeLeft->setMatrix(m);
    m.makeTranslate(1.0, 0.0, 0.0);
    planeRight->setMatrix(m);
    planeLeft->addChild(texPlaneLeft);
    planeRight->addChild(texPlaneRight);

    sceneGraphRoot->addChild(planeLeft);
    sceneGraphRoot->addChild(planeRight);
}


osg::ref_ptr<osg::Group> VXOculusViewer::createTexturePlane() {
    osg::ref_ptr<osg::Group> group = new osg::Group;
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec2Array> texture_coordinates = new osg::Vec2Array;
    osg::ref_ptr<osg::Vec4Array> colours = new osg::Vec4Array;

    vertices->push_back( osg::Vec3(-1.f,  1.f, 0.f));
    vertices->push_back( osg::Vec3(-1.f, -1.f, 0.f));
    vertices->push_back( osg::Vec3( 1.f, -1.f, 0.f));
    vertices->push_back( osg::Vec3( 1.f,  1.f, 0.f));
    geometry->setVertexArray(vertices.get());

    texture_coordinates->push_back(osg::Vec2(0.f, 0.f));
    texture_coordinates->push_back(osg::Vec2(0.f, 1.f));
    texture_coordinates->push_back(osg::Vec2(1.f, 1.f));
    texture_coordinates->push_back(osg::Vec2(1.f, 0.f));
    geometry->setTexCoordArray(0, texture_coordinates.get());

    colours->push_back(osg::Vec4(1.0, 1.0, 1.0, 1.0));
    geometry->setColorArray(colours.get());
    geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

    geometry->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
    geode->addDrawable(geometry.get());
    group->addChild(geode.get());

    return group.get();
}


/*
 * creates example scene - this can be any scene in Osg
 * just example to develop something
 */
osg::ref_ptr<osg::Node> VXOculusViewer::createExampleScene() {
    mt = new osg::MatrixTransform;
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec4Array> colours = new osg::Vec4Array;

    geometry->setVertexArray(vertices.get());
    vertices->push_back(osg::Vec3(-.5f, -.5f, 0.f));
    vertices->push_back(osg::Vec3( .5f, -.5f, 0.f));
    vertices->push_back(osg::Vec3( .5f,  .5f, 0.f));
    vertices->push_back(osg::Vec3(-.5f,  .5f, 0.f));

    geometry->setColorArray(colours.get());
    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    colours->push_back(osg::Vec4(1.f, 0.f, 0.f, 1.f));
    colours->push_back(osg::Vec4(0.f, 1.f, 0.f, 1.f));
    colours->push_back(osg::Vec4(0.f, 0.f, 1.f, 1.f));
    colours->push_back(osg::Vec4(1.f, 1.f, 1.f, 1.f));

    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
    geode->addDrawable(geometry.get());

    osg::Matrix m;
    m.makeRotate(0.0, osg::Vec3(0.0, 0.0, 1.0));
    mt->setMatrix(m);
    mt->addChild(geode);

    return mt.get();
}