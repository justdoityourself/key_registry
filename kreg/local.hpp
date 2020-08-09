/* Copyright (C) 2020 D8DATAWORKS - All Rights Reserved */

#pragma once

#include "mhttp/http.hpp"
#include "d8u/string_switch.hpp"

#include "d8u/crypto.hpp"

#include "database.hpp"

#include <string_view>
#include <filesystem>

#include <fstream>
#include <iterator>

namespace kreg
{
    using namespace mhttp;
    using namespace d8u;

    //TODO
    /*class Manager
    {
        std::string host;
        std::string name;

        crypto::KeyPair key;

    public:
        Manager(std::string_view _name, std::string_view password, std::string_view _host)
            : host(_host)
            , name(_name)
        {
            d8u::transform::Password pw(password);

            if (std::filesystem::exists(name + ".dat"))
            {
                std::ifstream f(name + ".dat", ios::binary);
                std::string key_str(std::istreambuf_iterator<char>{f}, {});

                d8u::transform::decrypt(key_str, pw);

                key.Import(key_str);
            }
            else
            {
                key = crypto::KeyPair::Create(4096);
                auto key_str = key.Export();
                d8u::transform::encrypt(key_str, pw);

                std::ofstream file(name + ".dat", ios::binary);
                file << key_str;

                HttpConnection(host).Post(std::string("/manager?id=") + name + "&cert=" + key.public_key.ExportSelfSigned(key.private_key), std::string_view());
            }
        }

        template < typename F > void Enumerate(F&& f)
        {
            auto _pool = HttpConnection(host).Get(std::string("/managed_pool?id=") + name, std::string_view());

            Helper stream(_pool.body);

            auto line = stream.GetLine();

            while (line)
            {
                auto [name, pw] = DecodeCredentials(line);

                f(name, *(d8u::transform::Password*) & pw);
                line = stream.GetLine();
            }
        }

    private:
        auto DecodeCredentials(std::string_view line)
        {
            auto bin = d8u::util::to_bin(line);

            auto dec = key.private_key.Decrypt(bin);
            std::string cred_str(dec.begin(), dec.end());

            auto f = cred_str.find('|');

            return std::make_pair(cred_str.substr(0, f), d8u::util::to_bin(cred_str.substr(f + 1)));
        }
    };*/

    class LocalGroup
    {
        Database db;
        std::string file;
        std::string id;
        std::string stream;

        d8u::transform::Password pw;

    public:
        LocalGroup(std::string_view _file, std::string_view password, std::string_view path, std::string_view manager = "")
            : db(path)
            , file(_file)
            , pw(password)
        {
            if (std::filesystem::exists(file + ".group"))
            {
                std::ifstream f(file + ".group", ios::binary);
                id = std::string(std::istreambuf_iterator<char>{f}, {});
            }
            else
            {
                std::array<uint8_t, 16> a;
                d8u::random_bytes_secure(a);

                id = d8u::util::to_hex(a);

                {
                    std::ofstream handle(file + ".group", ios::binary);
                    handle << id;
                }

                if (manager.size())
                    RequestManagement(manager);
            }

            if (std::filesystem::exists(file + ".stream"))
            {
                std::ifstream f(file + ".stream", ios::binary);
                stream = std::string(std::istreambuf_iterator<char>{f}, {});
            }
            else
            {
                std::array<uint8_t, 16> a;
                d8u::random_bytes_secure(a);

                stream = d8u::util::to_hex(a);

                {
                    std::ofstream handle(file + ".stream", ios::binary);
                    handle << stream;
                }

                db.JoinGroup(id, stream + "\r\n");
            }
        }

        template < typename F > void EnumerateGroup(F&& f)
        {
            auto stream_buffer = db.ReadGroupPool(id);
            Helper stream(stream_buffer);

            auto line = stream.GetLine();

            while (line)
            {
                f(std::string_view(line));
                line = stream.GetLine();
            }
        }

        template < typename F > void EnumerateStream(F&& f, std::string _stream = "")
        {
            if (!_stream.size())
                _stream = stream;

            auto stream_buffer = db.ReadStream(_stream);
            Helper stream(stream_buffer);

            auto line = stream.GetN(32);

            while (line)
            {
                f(std::string_view(line));
                line = stream.GetN(32);
            }
        }

        std::string GetElement(std::string element)
        {
            auto bin = d8u::util::to_bin(db.ReadElement(element));
            d8u::transform::decrypt(bin, pw);

            return std::string(bin.begin(), bin.end());
        }

        void AddElement(std::string description)
        {
            std::array<uint8_t, 16> a;
            d8u::random_bytes_secure(a);

            std::string element = d8u::util::to_hex(a);
            d8u::transform::encrypt(description, pw);

            db.AddElement(stream, element, d8u::util::to_hex(description));
        }

        void RequestManagement(std::string_view manager)
        {
            d8u::crypto::PublicPassword public_key;

            public_key.ImportSelfSigned(db.FindManager(manager));

            auto encrypted = public_key.Encrypt(id + "|" + d8u::util::to_hex(pw));

            auto finished = d8u::util::to_hex(encrypted);

            db.RequestManagement(manager, finished + "\r\n");
        }
    };
}
