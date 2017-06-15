#include "kis_watercolorop_test.h"

#include <QTest>
#include <qimage_based_test.h>

#include "plugins/paintops/watercolor/kis_wetmap.h"
#include "plugins/paintops/watercolor/kis_splat.h"

#include <QString>
#include <QVector>
#include <kis_filter_configuration.h>
#include "kis_painter.h"

class TestWetMap
{
public:
    TestWetMap(){}

    void test() {
        QVector<QPoint> pos;
        QVector<qreal> radius;

        pos.push_back(QPoint(10, 10));
        radius.push_back(5.5);
        pos.push_back(QPoint(50, 50));
        radius.push_back(25);
        pos.push_back(QPoint(63, 60));
        radius.push_back(19.36);
        test(pos, radius);

        test(256, QString("Full_Drying"));
        test(128, QString("Half_Drying"));
        test(64, QString("Quarter_Drying"));

        pos.clear();
        radius.clear();
        for (int i = 0; i < 3; i++)
            radius.push_back(50);
        pos.push_back(QPoint(50, 50));
        pos.push_back(QPoint(100, 50));
        pos.push_back(QPoint(150, 50));
        QVector<int> addingTime;
        addingTime.push_back(0);
        addingTime.push_back(64);
        addingTime.push_back(128);
        test(256, pos, radius, addingTime, "One_Line");
        test(300, pos, radius, addingTime, "One_Line_Overtime");

        pos.clear();
        pos.push_back(QPoint(50, 50));
        pos.push_back(QPoint(125, 50));
        pos.push_back(QPoint(100, 125));
        test(256, pos, radius, addingTime, "Triange");
        test(300, pos, radius, addingTime, "Triange_Overtime");

        test(0);
        test(64);
        test(128);
        test(256);

        test(300);
    }

    // Testing adding watter on wetmap
    void test(QVector<QPoint> pos, QVector<qreal> radius) {

        KisWetMap *wetMap = new KisWetMap();
        for (int i = 0; i < pos.size(); i++) {
            wetMap->addWater(pos.at(i), radius.at(i));
        }
        QVERIFY(TestUtil::checkQImage(wetMap->getPaintDevice()->convertToQImage(0),
                                      "WetMap",
                                      "multiple",
                                      "adding_water"));
        delete wetMap;
    }

    // Testing drying
    void test(int time, QString prefix) {
        KisWetMap *wetMap = new KisWetMap();
        QPoint pos(50, 50);
        qreal radius = 25;

        wetMap->addWater(pos, radius);

        for (int i = 0; i < time; i++) {
            wetMap->update();
        }

        QVERIFY(TestUtil::checkQImage(wetMap->getPaintDevice()->convertToQImage(0),
                                      "WetMap",
                                      prefix,
                                      "drying"));
        delete wetMap;
    }

    // Tesitng multiple actions with wetmap
    void test(int fullWorkTime, QVector<QPoint> pos, QVector<qreal> radius,
              QVector<int> addingTime, QString prefix) {
        KisWetMap *wetMap = new KisWetMap();
        int posI;
        for (int i = 0; i <= fullWorkTime; i++) {
            if ((posI = addingTime.indexOf(i)) != -1) {
                wetMap->addWater(pos.at(posI), radius.at(posI));
            }
            wetMap->update();
        }

        QVERIFY(TestUtil::checkQImage(wetMap->getPaintDevice()->convertToQImage(0),
                                      "WetMap",
                                      prefix,
                                      "multiple_actions"));
        delete wetMap;
    }

    // Testing reading watercount
    void test(int time) {
        KisWetMap *wetMap = new KisWetMap();
        wetMap->addWater(QPoint(50, 50), 50);
        for (int i = 0; i < time; i++)
            wetMap->update();
        QVector<QPointF> inPoint;
        inPoint.push_back(QPointF(75, 63));
        QVector<int> realVal = wetMap->getWater(inPoint);
        int expectedVal = 32767 - 128 * time;
        if (expectedVal < 0)
            expectedVal = 0;
        QVector<int> exp;
        exp.push_back(expectedVal);
        QCOMPARE(realVal, exp);

        inPoint.clear();
        inPoint.push_back(QPointF(57, 76));
        QVector<QPoint> realSpeed = wetMap->getSpeed(inPoint);
        QPoint expectedSpeed = (realVal.first() == 0) ? QPoint(0, 0) : QPoint(8518, 31640);
        QVector<QPoint> expVec;
        expVec.push_back(expectedSpeed);
        QCOMPARE (realSpeed, expVec);
    }
};

class TestSplat
{
public:
    TestSplat() {}

    void test() {
        test(QPoint(50, 50), 25);

        QVector<QPoint> pos;
        QVector<qreal> radius;
        QVector<KoColor> colors;
        for (int i = 0; i < 3; i++)
            radius.push_back(50);
        pos.push_back(QPoint(50, 50));
        pos.push_back(QPoint(100, 50));
        pos.push_back(QPoint(150, 50));
        KoColor clr;
        clr.fromQColor(QColor(Qt::red));
        colors.push_back(clr);
        clr.fromQColor(QColor(Qt::green));
        colors.push_back(clr);
        clr.fromQColor(QColor(Qt::blue));
        colors.push_back(clr);

        test(pos, radius, colors, 24, "full_lifetime_line");

        pos.clear();
        pos.push_back(QPoint(50, 50));
        pos.push_back(QPoint(125, 50));
        pos.push_back(QPoint(100, 125));

        test(pos, radius, colors, 24, "full_lifetime_triange");

        test(QPoint(50, 50), 50,
             QPoint(0, 0), 75);
    }

    // Test one splat
    void test(QPoint pos, qreal radius) {
        KoColor clr;
        clr.fromQColor(QColor(Qt::red));
        KisSplat *splat = new KisSplat(pos, 2*radius, clr);
        KisWetMap *wetMap = new KisWetMap();
        wetMap->addWater(pos, 2*radius);
        for (int i = 0; i < 20; i++) {
            splat->update(wetMap);
            wetMap->update();
        }

        KisPaintDeviceSP dev = new KisPaintDevice(KoColorSpaceRegistry::instance()->rgb16());
        KisPainter *painter = new KisPainter(dev);
        splat->doPaint(painter);
        QVERIFY(!TestUtil::checkQImage(dev->convertToQImage(0),
                                      "splat",
                                      "one_splat",
                                      ""));
    }

    // Test making different splats with different colors
    void test(QVector<QPoint> pos, QVector<qreal> radius, QVector<KoColor> colors, int time, QString prefix) {
        QVector<KisSplat *> splats;
        KisWetMap *wetMap = new KisWetMap();
        for (int i = 0; i < pos.size(); i++) {
            splats.push_back(new KisSplat(pos.at(i),
                                          2 * radius.at(i),
                                          colors.at(i)));
            wetMap->addWater(pos.at(i),
                             radius.at(i) + 5);
        }

        for (int i = 0; i < time; i++) {
            foreach (KisSplat *splat, splats)
                splat->update(wetMap);
            wetMap->update();
        }

        KisPaintDeviceSP dev = new KisPaintDevice(KoColorSpaceRegistry::instance()->rgb16());
        KisPainter *painter = new KisPainter(dev);
        foreach (KisSplat *splat, splats) {
            splat->doPaint(painter);
        }
        QVERIFY(!TestUtil::checkQImage(dev->convertToQImage(0),
                                      "splat",
                                      "many_splats",
                                      prefix));
    }

    // Test flowing part of splat
    void test(QPoint posSplat, qreal radiusSplat, QPoint posWetMap, qreal radiusWetMap) {
        KoColor clr;
        clr.fromQColor(QColor(Qt::red));
        KisSplat *splat = new KisSplat(posSplat, 2*radiusSplat, clr);
        KisWetMap *wetMap = new KisWetMap();
        wetMap->addWater(posSplat, radiusSplat+1);
        wetMap->addWater(posWetMap, radiusWetMap);
        for (int i = 0; i < 30; i++) {
            splat->update(wetMap);
            wetMap->update();
        }

        KisPaintDeviceSP dev = new KisPaintDevice(KoColorSpaceRegistry::instance()->rgb16());
        KisPainter *painter = new KisPainter(dev);
        splat->doPaint(painter);
        QVERIFY(!TestUtil::checkQImage(dev->convertToQImage(0),
                                      "splat",
                                      "flowing_part",
                                      "one_adding_water"));
    }

    // Test reweting splat
    void test(QPoint splatPos, qreal splatRadius, QPoint rewetPos, qreal rewetRadius, int rewetingTime) {
        KoColor clr;
        clr.fromQColor(QColor(Qt::red));
        KisSplat *splat = new KisSplat(splatPos, 2*splatRadius, clr);
        KisWetMap *wetMap = new KisWetMap();
        wetMap->addWater(splatPos, splatRadius);

        for (int i = 0; i < 30; i++) {
            splat->update(wetMap);
            wetMap->update();
        }

        wetMap->addWater(rewetPos, rewetRadius);
        splat->rewet(wetMap, rewetPos, rewetRadius);

        for (int i = 0; i < rewetingTime; i++) {
            splat->update(wetMap);
            wetMap->update();
        }

        KisPaintDeviceSP dev = new KisPaintDevice(KoColorSpaceRegistry::instance()->rgb16());
        KisPainter *painter = new KisPainter(dev);
        splat->doPaint(painter);
        QVERIFY(!TestUtil::checkQImage(dev->convertToQImage(0),
                                      "splat",
                                      "reweting",
                                      "reweting_once"));
    }
};

void WaterColorTest::testWetMap()
{
    TestWetMap t;
    t.test();
}

void WaterColorTest::testSplat()
{
    TestSplat t;
    t.test();
}

QTEST_MAIN(WaterColorTest)