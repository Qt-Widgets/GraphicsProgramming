/* Copyright (C) 2013-2015 Karl Phillip Buhr <karlphillip@gmail.com>
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike License.
 * To view a copy of this license, visit:
 *      https://creativecommons.org/licenses/by-sa/2.5/legalcode
 *
 * Or to read the human-readable summary of the license:
 *      https://creativecommons.org/licenses/by-sa/2.5/
 */
#include "serialportreader.h"
#include <iostream>

#include <QCoreApplication>
#include <QDebug>
#include <QByteArray>
#include <QTextCodec>


SerialPortReader::SerialPortReader(QSerialPort* serial, QObject* parent)
: QObject(parent), _serial(serial)
{
    // The magic call that makes QSerialPort outputs from a serial connected with Arduino
    _serial->setDataTerminalReady(true);

    QObject::connect(_serial, SIGNAL(readyRead()),
                     SLOT(_read_callback()));

    QObject::connect(_serial, SIGNAL(error(QSerialPort::SerialPortError)),
                     SLOT(_error_callback(QSerialPort::SerialPortError)));
}

SerialPortReader::~SerialPortReader()
{
}

void SerialPortReader::_read_callback()
{
    if (!_serial)
        return;

    QByteArray data = _serial->readAll();
    int sz = data.size();

    /* On my Arduino data is always 3 bytes:
     *  data[0]: is the value I'm interested at
     *  data[1]: is always 13 (carriage return)
     *  data[2]: is always 10 (new line)
     *
     * which causes std::cout to jump 2 lines, and I find it annoying,
     * hence the *if* statement below just to print the value I need.
     */
    if (sz == 3 && (int)data[1] == 13 && (int)data[2] == 10)
    {
        std::cout << data[0] << std::endl;
    }
    else
    {
        // To print data from other serial devices
        QString str = QString::fromUtf8(data);
        std::cout << str.toStdString();
    }
}

void SerialPortReader::_error_callback(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ReadError)
    {
        std::cout << "!!! An I/O error occurred while reading the data from port " << _serial->portName().toStdString() <<
                     ". Error: " << _serial->errorString().toStdString() << std::endl;
        QCoreApplication::exit(1);
    }
}
