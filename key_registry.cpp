#include "kreg/service.hpp"
#include "kreg/client.hpp"

int main()
{
    kreg::Service svc("8090","test.db");

    kreg::Manager m("pool5123", "password", "127.0.0.1:8090");

    kreg::Node n("node", "PASSWORD", "pool5123","127.0.0.1:8090");

    m.Enumerate([&](auto& user, auto& password)
    {

    });

    svc.Join();

    return 0;
}
