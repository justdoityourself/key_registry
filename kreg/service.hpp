/* Copyright (C) 2020 D8DATAWORKS - All Rights Reserved */

#pragma once

#include "mhttp/http.hpp"
#include "d8u/string_switch.hpp"

#include "database.hpp"

#include <string_view>

namespace kreg
{
	using namespace mhttp;
    using namespace d8u;

	class Service 
        : public HttpServer 
        , public Database
	{
	public:
        Service(std::string_view db)
			: Database(db)
            , HttpServer([&](auto& c, auto&& req, auto* mplex)
			{
                switch (switch_t(req.type))
                {
                default:
                    return c.Http400();

                case switch_t("GET"):
                case switch_t("Get"):
                case switch_t("get"):

                    switch (switch_t(req.path))
                    {
                    default:
                        return c.Http400();
                    case switch_t("/manager"):
                        return c.Response("200 OK", FindManager(req.parameters["id"]), std::string_view("Content-Type: text/plain\r\n"));

                    case switch_t("/managed_pool"):
                        return c.Response("200 OK", ReadManagementPool(req.parameters["id"]), std::string_view("Content-Type: text/plain\r\n"));

                    case switch_t("/group"):
                        return c.Response("200 OK", ReadGroupPool(req.parameters["id"]), std::string_view("Content-Type: text/plain\r\n"));

                    case switch_t("/stream"):
                        return c.Response("200 OK", ReadStream(req.parameters["id"]), std::string_view("Content-Type: text/plain\r\n"));

                    case switch_t("/element"):
                        return c.Response("200 OK", ReadElement(req.parameters["id"]), std::string_view("Content-Type: text/plain\r\n"));
                    }

                case switch_t("POST"):
                case switch_t("Post"):
                case switch_t("post"):

                    switch (switch_t(req.path))
                    {
                    default:
                        return c.Http400();
                    case switch_t("/manager"):
                        AddManager(req.parameters["id"], req.parameters["cert"]);

                        return c.Http200();
                    case switch_t("/request_manager"):
                        RequestManagement(req.parameters["id"], std::string(req.parameters["cred"]) + "\r\n");

                        return c.Http200();
                    case switch_t("/join_group"):
                        JoinGroup(req.parameters["id"], std::string(req.parameters["stream"]) + "\r\n");

                        return c.Http200();
                    case switch_t("/add_element"):
                        AddElement(std::string(req.parameters["stream"]),req.parameters["id"], std::string(req.parameters["desc"]));

                        return c.Http200();
                    }
                }
			}) { }

		Service(std::string_view port, std::string_view db) : Service(db)
		{
            HttpServer::Open((uint16_t)std::stoi(port.data()));
		}
	};
}