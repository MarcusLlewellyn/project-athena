//
//  AnimVariantMap.cpp
//  library/animation
//
//  Created by Howard Stearns on 10/15/15.
//  Copyright (c) 2015 High Fidelity, Inc. All rights reserved.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include <QScriptEngine>
#include <QScriptValueIterator>
#include <QThread>
#include <RegisteredMetaTypes.h>
#include "AnimVariant.h" // which has AnimVariant/AnimVariantMap

const AnimVariant AnimVariant::False = AnimVariant();

QScriptValue AnimVariantMap::animVariantMapToScriptValue(QScriptEngine* engine, const QStringList& names, bool useNames) const {
    if (QThread::currentThread() != engine->thread()) {
        qCWarning(animation) << "Cannot create Javacript object from non-script thread" << QThread::currentThread();
        Q_ASSERT(false);
        return QScriptValue();
    }
    QScriptValue target = engine->newObject();
    auto setOne = [&] (const QString& name, const AnimVariant& value) {
        switch (value.getType()) {
            case AnimVariant::Type::Bool:
                target.setProperty(name, value.getBool());
                break;
            case AnimVariant::Type::Int:
                target.setProperty(name, value.getInt());
                break;
            case AnimVariant::Type::Float:
                target.setProperty(name, value.getFloat());
                break;
            case AnimVariant::Type::String:
                target.setProperty(name, value.getString());
                break;
            case AnimVariant::Type::Vec3:
                target.setProperty(name, vec3toScriptValue(engine, value.getVec3()));
                break;
            case AnimVariant::Type::Quat:
                target.setProperty(name, quatToScriptValue(engine, value.getQuat()));
                break;
            default:
                // Unknown type
                assert(QString("AnimVariant::Type") == QString("valid"));
        }
    };
    if (useNames) { // copy only the requested names
        for (const QString& name : names) {
            auto search = _map.find(name);
            if (search != _map.end()) { // scripts are allowed to request names that do not exist
                setOne(name, search->second);
            }
        }

    } else {  // copy all of them
        for (auto& pair : _map) {
            setOne(pair.first, pair.second);
        }
    }
    return target;
}
void AnimVariantMap::copyVariantsFrom(const AnimVariantMap& other) {
    for (auto& pair : other._map) {
        _map[pair.first] = pair.second;
    }
}

void AnimVariantMap::animVariantMapFromScriptValue(const QScriptValue& source) {
    if (QThread::currentThread() != source.engine()->thread()) {
        qCWarning(animation) << "Cannot examine Javacript object from non-script thread" << QThread::currentThread();
        Q_ASSERT(false);
        return;
    }
    // POTENTIAL OPTIMIZATION: cache the types we've seen. I.e, keep a dictionary mapping property names to an enumeration of types.
    // Whenever we identify a new outbound type in animVariantMapToScriptValue above, or a new inbound type in the code that follows here,
    // we would enter it into the dictionary. Then switch on that type here, with the code that follow being executed only if
    // the type is not known. One problem with that is that there is no checking that two different script use the same name differently.
    QScriptValueIterator property(source);
    // Note: QScriptValueIterator iterates only over source's own properties. It does not follow the prototype chain.
    while (property.hasNext()) {
        property.next();
        QScriptValue value = property.value();
        if (value.isBool()) {
            set(property.name(), value.toBool());
        } else if (value.isString()) {
            set(property.name(), value.toString());
        } else if (value.isNumber()) {
            int asInteger = value.toInt32();
            float asFloat = value.toNumber();
            if (asInteger == asFloat) {
                set(property.name(), asInteger);
            } else {
                set(property.name(), asFloat);
            }
        } else { // Try to get x,y,z and possibly w
            if (value.isObject()) {
                QScriptValue x = value.property("x");
                if (x.isNumber()) {
                    QScriptValue y = value.property("y");
                    if (y.isNumber()) {
                        QScriptValue z = value.property("z");
                        if (z.isNumber()) {
                            QScriptValue w = value.property("w");
                            if (w.isNumber()) {
                                set(property.name(), glm::quat(x.toNumber(), y.toNumber(), z.toNumber(), w.toNumber()));
                            } else {
                                set(property.name(), glm::vec3(x.toNumber(), y.toNumber(), z.toNumber()));
                            }
                            continue; // we got either a vector or quaternion object, so don't fall through to warning
                        }
                    }
                }
            }
            qCWarning(animation) << "Ignoring unrecognized data" << value.toString() << "for animation property" << property.name();
            Q_ASSERT(false);
        }
    }
}
