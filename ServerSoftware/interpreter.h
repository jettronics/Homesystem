#ifndef INTERPRETER_H
#define INTERPRETER_H

class Control;
class ProcMessage;

class Interpreter
{
public:
   Interpreter(ProcMessage *procmsg, Control *controller);
   virtual ~Interpreter();

public:
   virtual void interprete()=0;
   void response(string content);
   static void init(vector<ProcMessage*> *procmsg, Control *controller);
   static void deinit();
   static void call();

protected:
   Control *controller;
   static ProcMessage *consolemsg;
   ProcMessage *procmsg;

private:
   static vector<Interpreter*> interVect;

};


class RemoteCtrl : public Interpreter
{
public:
   RemoteCtrl(ProcMessage *procmsg, Control *controller);
   virtual ~RemoteCtrl();

public:
   void interprete();

private:
   SpiMessage *spimsg;
   unsigned char previousrxdata;

};


class WebServer : public Interpreter
{
public:
   WebServer(ProcMessage *procmsg, Control *controller);
   virtual ~WebServer();

public:
   void interprete();

};


class WebCtrl : public Interpreter
{
public:
   WebCtrl(ProcMessage *procmsg, Control *controller);
   virtual ~WebCtrl();

public:
   void interprete();

};



#endif 
