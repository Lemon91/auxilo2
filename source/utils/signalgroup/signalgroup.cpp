#include "signalgroup.h"

namespace Utils {

SignalGroup::SignalGroup(QString name, GroupType type, QObject* parent) :
    QObject(parent),
    m_client(NULL),
    m_exchange(NULL),
    m_queue(NULL),
    m_name(name),
    m_type(type),
    m_queueReady(false),
    m_exchangeReady(false)
{
   m_client = Connection::getClient();

   switch (type) {
   case Publisher:
      m_queueReady = true;
      enableEmitting();
      break;
   case Subscriber:
      m_exchangeReady = true;
      enableReceiving();
      break;
   case Both:
      enableEmitting();
      enableReceiving();
      break;
   default:
      qCritical("SignalGroup::SignalGroup(): "
                "Invalid GroupType: %i", type);
   }
}

SignalGroup::~SignalGroup()
{
}

bool SignalGroup::publish(QByteArray data)
{
   if (!m_exchange) {
      return false;
   }

   m_exchange->publish(data, "", "application/octet-stream");

   return false;
}

void SignalGroup::onQueueDeclared()
{
   m_queue->bind(m_name, "");
   m_queue->consume(QAMQP::Queue::coNoAck);

   m_queueReady = true;
   if (m_exchangeReady) {
      emit ready();
   }
}

void SignalGroup::onExchangeDeclared()
{
   m_exchangeReady = true;
   if (m_queueReady) {
      emit ready();
   }
}

void SignalGroup::onMessageReceived(QAMQP::Queue* queue)
{
   QByteArray payload = queue->getMessage()->payload;
   emit messageReceived(payload, m_name);
}

void SignalGroup::enableEmitting()
{
   m_exchange = m_client->createExchange(m_name);
   m_exchange->declare("fanout");

   connect(m_exchange, SIGNAL(declared()), this, SLOT(onExchangeDeclared()));
}

void SignalGroup::enableReceiving()
{
   m_queue = m_client->createQueue();
   m_queue->declare("", QAMQP::Queue::Exclusive);

   connect(m_queue, SIGNAL(declared()), this, SLOT(onQueueDeclared()));
   connect(m_queue, SIGNAL(messageReceived(QAMQP::Queue*)),
           this,  SLOT(onMessageReceived(QAMQP::Queue*)));
}

} // Utils
