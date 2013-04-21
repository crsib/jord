#pragma once;

#include <cstdint>

#include "rc/File.h"
#include "rc/Directory.h"

namespace rc
{
	class Root : public std::enable_shared_from_this<Root>
	{
		Root( const Root&& );
		Root& operator = ( const Root& );
	public:

		struct TOCItem
		{
			enum TOCItemType
			{
				Directory,
				File
			};

			TOCItemType   Type;
			const char*   Name;

			union
			{
				size_t    EntriesCount;
				size_t    Offset;
			} ;       

			size_t         FileSize;
		};

		Root( const TOCItem* toc, uint8_t const * const base_address );
		virtual ~Root();

		NodePtr      find( const std::string& path ) const;

		void const*  getFilePointer( const std::string& path ) const
		{ getFilePointer( find( path ) ); }

		void const*  getFilePointer( const NodePtr& node ) const;

		size_t       getFileSize( const std::string& path ) const
		{ getFileSize( find( path) ); }

		size_t       getFileSize( const NodePtr& node ) const;

		Directory*   getRootDirectory() const { return m_RootDirectory; }
	private:
		uint8_t const*         m_BaseAddress;

		Directory*             m_RootDirectory;
	};

	typedef std::shared_ptr< Root > RootPtr;
}