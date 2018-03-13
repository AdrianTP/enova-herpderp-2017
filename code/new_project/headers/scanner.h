/*
 * =====================================================================================
 *
 *       Filename:  scanner.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/19/2017 16:58:37
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef SCANNER_H
#define SCANNER_H

class Scanner {
  private:
    // Vars
    unsigned long previousMillis;
    int interval;
    int direction;

  public:
    // Constructor
    Scanner();

    // Destructor
    virtual ~Scanner();

    void tick();
}
