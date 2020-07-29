/* Copyright (C) 2020 D8DATAWORKS - All Rights Reserved */

#pragma once

#include <string_view>

#include "tdb/tdb.hpp"

namespace kreg
{
	using namespace tdb;

	using R = AsyncMap<4 * 1024 * 1024, 256 * 1024>;
	using N = SimpleFuzzyHashBuilder<256 * 1024, uint64_t, Key32, 8, 2>;

	using DB = DatabaseBuilder < R, Stream< R, BTree< R, N> > >;

	class Database : DB
	{
	public:
		Database(std::string_view file)
			: DB(file) { }



		/*
			Credential Sharing API
		*/

		void AddManager(std::string_view manager, std::string_view certificate)
		{
			auto table = Table<0>();

			table.WriteLock(std::string(manager) + "_manager", certificate);
		}

		std::string FindManager(std::string_view manager)
		{
			auto table = Table<0>();

			auto certificate = table.Read(std::string(manager) + "_manager");

			return std::string(certificate.begin(),certificate.end());
		}

		void RequestManagement(std::string_view manager, std::string_view credentials)
		{
			auto table = Table<0>();

			table.WriteLock(std::string(manager) + "_credentials", credentials);
		}

		std::string ReadManagementPool(std::string_view manager)
		{
			/*
				TODO: Signed Request Timestamp
			*/

			auto table = Table<0>();
			
			auto pool = table.Read(std::string(manager) + "_credentials");

			return std::string(pool.begin(), pool.end());
		}



		/*
			Stream Group API
		*/

		void JoinGroup(std::string_view group, std::string_view stream)
		{
			auto table = Table<0>();

			table.WriteLock(std::string(group) + "_group", stream);
		}

		std::string ReadGroupPool(std::string_view group)
		{
			auto table = Table<0>();

			auto pool = table.Read(std::string(group) + "_group");

			return std::string(pool.begin(), pool.end());
		}



		/*
			Stream API
		*/

		void AddElement(std::string_view stream, std::string_view element, std::string_view description)
		{
			auto table = Table<0>();

			table.WriteLock(std::string(stream) + "_stream", element);

			table.WriteLock(std::string(element) + "_element", description);
		}

		std::string ReadStream(std::string_view stream)
		{
			auto table = Table<0>();

			auto pool = table.Read(std::string(stream) + "_stream");

			return std::string(pool.begin(), pool.end());
		}

		std::string ReadElement(std::string_view element)
		{
			auto table = Table<0>();

			auto pool = table.Read(std::string(element) + "_element");

			return std::string(pool.begin(), pool.end());
		}
	};
}