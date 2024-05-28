#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

// Code took from ItzKiba/BetterProgression, thanks!

inline static CCParticleSystem* createParticles(int numParticles, ccColor3B startColor, ccColor3B endColor, std::string emitterType, std::string duration, std::string particleLifespan, std::string particleLifespanVariance, std::string angle, std::string angleVariance, std::string sourcePositionx, std::string sourcePositiony, std::string sourcePositionVariancex, std::string sourcePositionVariancey, std::string maxRadius, std::string minRadius, std::string startParticleSize, std::string finishParticleSize, std::string startParticleSizeVariance, std::string speed, std::string speedVariance, std::string gravityx, std::string gravityy, std::string startColorAlpha, std::string startColorVarianceRed, std::string startColorVarianceBlue, std::string startColorVarianceGreen, std::string startColorVarianceAlpha, std::string finishColorAlpha, std::string finishColorVarianceRed, std::string finishColorVarianceBlue, std::string finishColorVarianceGreen, std::string finishColorVarianceAlpha, std::string rotatePerSecond, std::string rotatePerSecondVariance, std::string rotationEndVariance, std::string textureFileName) {
    auto dict = CCDictionary::createWithContentsOfFileThreadSafe("dragEffect.plist");

    dict->setObject(CCString::create(emitterType), "emitterType");
    dict->setObject(CCString::create(duration), "duration");
    dict->setObject(CCString::createWithFormat("%i", numParticles), "maxParticles");
    dict->setObject(CCString::create(particleLifespan), "particleLifespan");
    dict->setObject(CCString::create(particleLifespanVariance), "particleLifespanVariance");
    dict->setObject(CCString::create(angle), "angle");
    dict->setObject(CCString::create(angleVariance), "angleVariance");
    dict->setObject(CCString::create(sourcePositionx), "sourcePositionx");
    dict->setObject(CCString::create(sourcePositiony), "sourcePositiony");
    dict->setObject(CCString::create(sourcePositionVariancex), "sourcePositionVariancex");
    dict->setObject(CCString::create(sourcePositionVariancey), "sourcePositionVariancey");
    dict->setObject(CCString::create(maxRadius), "maxRadius");
    dict->setObject(CCString::create(minRadius), "minRadius");
    dict->setObject(CCString::create(startParticleSize), "startParticleSize");
    dict->setObject(CCString::create(finishParticleSize), "finishParticleSize");
    dict->setObject(CCString::create(startParticleSizeVariance), "startParticleSizeVariance");
    dict->setObject(CCString::create(speed), "speed");
    dict->setObject(CCString::create(speedVariance), "speedVariance");
    dict->setObject(CCString::create(gravityx), "gravityx");
    dict->setObject(CCString::create(gravityy), "gravityy");
    dict->setObject(CCString::create(fmt::format("{}", startColor.r)), "startColorRed");
    dict->setObject(CCString::create(fmt::format("{}", startColor.g)), "startColorGreen");
    dict->setObject(CCString::create(fmt::format("{}", startColor.b)), "startColorBlue");
    dict->setObject(CCString::create(startColorAlpha), "startColorAlpha");
    dict->setObject(CCString::create(startColorVarianceRed), "startColorVarianceRed");
    dict->setObject(CCString::create(startColorVarianceBlue), "startColorVarianceBlue");
    dict->setObject(CCString::create(startColorVarianceGreen), "startColorVarianceGreen");
    dict->setObject(CCString::create(startColorVarianceAlpha), "startColorVarianceAlpha");
    dict->setObject(CCString::create(fmt::format("{}", endColor.r)), "finishColorRed");
    dict->setObject(CCString::create(fmt::format("{}", endColor.g)), "finishColorGreen");
    dict->setObject(CCString::create(fmt::format("{}", endColor.b)), "finishColorBlue");
    dict->setObject(CCString::create(finishColorAlpha), "finishColorAlpha");
    dict->setObject(CCString::create(finishColorVarianceRed), "finishColorVarianceRed");
    dict->setObject(CCString::create(finishColorVarianceBlue), "finishColorVarianceBlue");
    dict->setObject(CCString::create(finishColorVarianceGreen), "finishColorVarianceGreen");
    dict->setObject(CCString::create(finishColorVarianceAlpha), "finishColorVarianceAlpha");
    dict->setObject(CCString::create(rotatePerSecond), "rotatePerSecond");
    dict->setObject(CCString::create(rotatePerSecondVariance), "rotatePerSecondVariance");
    dict->setObject(CCString::create(rotationEndVariance), "rotationEndVariance");
    dict->setObject(CCString::create(textureFileName), "textureFileName");

    auto emitter = CCParticleSystemQuad::create();
    emitter->initWithDictionary(dict, false);
    
    return emitter;
}