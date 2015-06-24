//
//  MovingPercentileTests.h
//  tests/shared/src
//
//  Created by Yixin Wang on 6/4/2014
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_MovingPercentileTests_h
#define hifi_MovingPercentileTests_h

#include <QtTest/QtTest>

class MovingPercentileTests : public QObject {
    Q_OBJECT

private slots:
    void runAllTests();

private:
    float random();
};

#endif // hifi_MovingPercentileTests_h
