// DecompressLibrary.h  Version 1.0
//
// Authors:      Marius Ciorecan (cmaryus@yahoo.com)
//
//
// Version 1.0   - Using XUnzip and gzip classes to make a general class that could decompress a zip, gz, tar.gz
// //
///////////////////////////////////////////////////////////////////////////////
//
// Marius Ciorecan comments:
// --------------------------
// The class uses the code from :
//		- XUnzip library written by Hans Dietrich (hdietrich2@hotmail.com) for decompression of zip files.
//		- CGzip library written by Jonathan de Halleux (dehalleux@pelikhan.com)
//	All other source files from this library contain the original source code, see there the copyright
//	The class uses also the specifications for the TAR format, in order to extract data from a tar file.

#include "decompressclass.h"

CDecompressClass::CDecompressClass(void)
{
	m_hZIP = NULL;
	m_pFileBuffer = NULL;
	m_szLastError = "";
	m_nArchiveType = UNKNOWN_ARCHIVE;
}

CDecompressClass::~CDecompressClass(void)
{
	CloseArchive();
}

BOOL CDecompressClass::OpenArchive(IN const char* pszArchiveFile)
{
	// let's try to found out what archive we have....

	// check if it's zip...
	m_hZIP = OpenZip((void*)pszArchiveFile, 0, ZIP_FILENAME);
	if (m_hZIP != NULL)
	{
		// ok, it's a zip !
		m_nArchiveType = ZIP_ARCHIVE;
		return TRUE;
	}
	
	// check if it's a gzip
	if (m_GZIP.Open(pszArchiveFile, CGZip::ArchiveModeRead))
	{
		// so far it's a gzip

		// read the buffer
		if (m_pFileBuffer == NULL)
		{
			// first read the file
 
			if (!m_GZIP.ReadBuffer((void**)&m_pFileBuffer, m_nBufferLen))
			{
				m_szLastError = "Could not read data from gzip archive !";
				return FALSE;
			}
			if (m_pFileBuffer == NULL)
			{
				m_szLastError = "Could not read data from gzip archive !";
				return FALSE;
			}
		}

		// check if it's a tar
		struct posix_header* pTarBlock;
		int i;
		unsigned long checksum;
		char* pBuffer = m_pFileBuffer;
		pTarBlock = (struct posix_header*)((char *)pBuffer);
		checksum = 0;
		for(i = 0; i < 512; i++)
		{
			// the chksum positions are treated as blanks
			if ((i >= 148) && (i < 156))
			{
				checksum += 32;
			}
			else
			{
				checksum += 0xFF & pBuffer[i];
			}
		}
		if (checksum == getval8(pTarBlock->chksum, 8))
		{
			// ok, it's a tar !
			m_nArchiveType = TAR_GZIP_ARCHIVE;
			return TRUE;
		}

		// we reach here, it's a gzip only because we failed the header check
		m_nArchiveType = GZIP_ARCHIVE;
		return TRUE;
	}

	// we reach here -> could not found what archive is this :(
	m_szLastError = "Could not determine archive type !";
	return FALSE;
}

void CDecompressClass::CloseArchive()
{
	if (m_hZIP != NULL)
	{
		CloseZip(m_hZIP);
		m_hZIP = NULL;
	}
	if (m_GZIP.IsOpen())
	{
		m_GZIP.Close();
	}
	if (m_pFileBuffer != NULL)
	{
		delete[] m_pFileBuffer;
		m_pFileBuffer = NULL;
	}
	m_nBufferLen = 0;
	m_szLastError = "";
}

int	CDecompressClass::GetCompressedFilesCount()
{
	if (m_nArchiveType == UNKNOWN_ARCHIVE)
	{
		m_szLastError = "Archive not opened";
		return -1;
	}
	if (m_nArchiveType == ZIP_ARCHIVE)
	{
		if (m_hZIP == NULL)
		{
			return -1;
		}
		ZIPENTRY ze;
		ZRESULT zr = GetZipItem(m_hZIP, -1, &ze);
		if (zr != ZR_OK)
		{
			m_szLastError = "Could not get files from the archive !";
			return -1;
		}
		return ze.index;
	}
	else if (m_nArchiveType == GZIP_ARCHIVE)
	{
		// for a gzip we have always 1 file
		return 1;
	}
	else if (m_nArchiveType == TAR_GZIP_ARCHIVE)
	{
		// in this case we read first data from gz -> it should be a tar file
		// next we will parse the tar file to check how many files it contains
		
		if (m_pFileBuffer == NULL)
		{
			// first read the file
			if (!m_GZIP.ReadBuffer((void**)&m_pFileBuffer, m_nBufferLen))
			{
				m_szLastError = "Could not read data from gzip archive !";
				return -1;
			}
			if (m_pFileBuffer == NULL)
			{
				m_szLastError = "Could not read data from gzip archive !";
				return -1;
			}
		}

		// now parse this file to get number of files contained
		int nFilesCount = 0;
		struct posix_header* pTarBlock;
		int nFileSize = 0;
		int i;
		unsigned long checksum;
		char* pBuffer = m_pFileBuffer;
		int nFullContent = 0;
		
		while (true)
		{
			pTarBlock = (struct posix_header*)((char *)pBuffer);
			nFileSize = getval8(pTarBlock->size, 12);
			checksum = 0;
			for(i = 0; i < 512; i++)
			{
				// the chksum positions are treated as blanks
				if ((i >= 148) && (i < 156))
				{
					checksum += 32;
				}
				else
				{
					checksum += 0xFF & pBuffer[i];
				}
			}
			if (checksum != getval8(pTarBlock->chksum, 8))
			{
				break;
			}

			// ok, it's a valid header, move over the content
			// the content is rounded over the file length to contain fixed number of 512 blocks
			nFullContent = ((nFileSize + 511) & ~511);

			pBuffer += nFullContent + 512;

			nFilesCount++;
		}

		return nFilesCount;
	}
	return -1;
}

BOOL CDecompressClass::GetArchiveFile(IN int nFileIndex, 
									  OUT char** pszFileBuffer, 
									  OUT int& nBufferSize,
									  OUT BOOL& fIsFile,
									  OUT CString& szFileName)
{
	if (m_nArchiveType == UNKNOWN_ARCHIVE)
	{
		m_szLastError = "Archive not opened";
		return FALSE;
	}
	if (m_nArchiveType == ZIP_ARCHIVE)
	{
		if (m_hZIP == NULL)
		{
			return FALSE;
		}
		ZIPENTRY ze;
		ZRESULT zr = GetZipItem(m_hZIP, nFileIndex, &ze);
		if (zr != ZR_OK)
		{
			m_szLastError = "Could not get file at the specified position from the archive !";
			return FALSE;
		}

		// if the buffer was previously allocated -> delete it
		if (m_pFileBuffer != NULL)
		{
			delete[] m_pFileBuffer;
			m_pFileBuffer = NULL;
		}

		m_pFileBuffer = new char[ze.unc_size + 10];
		zr = UnzipItem(m_hZIP, nFileIndex, m_pFileBuffer, ze.unc_size, ZIP_MEMORY);
		if ((zr != ZR_OK) && (zr != ZR_MORE))
		{
			m_szLastError = "Could not unzip file at the specified position from the archive !";
			return FALSE;
		}
		nBufferSize = ze.unc_size;
		m_pFileBuffer[nBufferSize] = 0;
		*pszFileBuffer = m_pFileBuffer;	
		if (ze.attr & FILE_ATTRIBUTE_NORMAL)
		{
			fIsFile = TRUE;
		}
		else
		{
			fIsFile = FALSE;
		}
		szFileName = ze.name;
	}
	else if (m_nArchiveType == GZIP_ARCHIVE)
	{
		// if the buffer is already open, don't read it
		if (m_pFileBuffer == NULL)
		{
			if (!m_GZIP.ReadBuffer((void**)&m_pFileBuffer, m_nBufferLen))
			{
				m_szLastError = "Could not read data from gzip archive !";
				return FALSE;
			}
		}
		nBufferSize = m_nBufferLen;
		*pszFileBuffer = m_pFileBuffer;
		fIsFile = TRUE;
		szFileName = "unknown";
	}
	else if (m_nArchiveType == TAR_GZIP_ARCHIVE)
	{
		// here we should have already the buffer
		if (m_pFileBuffer == NULL)
		{
			// first read the file
			if (!m_GZIP.ReadBuffer((void**)&m_pFileBuffer, m_nBufferLen))
			{
				m_szLastError = "Could not read data from gzip archive !";
				return FALSE;
			}

			if (m_pFileBuffer == NULL)
			{
				m_szLastError = "Could not read data from gzip archive !";
				return FALSE;
			}
		}

		// now parse this file to the position needed
		int nFilesCount = 0;
		struct posix_header* pTarBlock;
		int nFileSize = 0;
		int i;
		unsigned long checksum;
		char* pBuffer = m_pFileBuffer;
		int nFullContent = 0;

		for (int j=0; j < nFileIndex; j++)
		{
			pTarBlock = (struct posix_header*)((char *)pBuffer);
			nFileSize = getval8(pTarBlock->size, 12);
			checksum = 0;
			for(i = 0; i < 512; i++)
			{
				// the chksum positions are treated as blanks
				if ((i >= 148) && (i < 156))
				{
					checksum += 32;
				}
				else
				{
					checksum += 0xFF & pBuffer[i];
				}
			}
			if (checksum != getval8(pTarBlock->chksum, 8))
			{
				m_szLastError = "Invalid file position !";
				return FALSE;
			}

			// ok, it's a valid header, move over the content
			// the content is rounded over the file length to contain fixed number of 512 blocks
			nFullContent = ((nFileSize + 511) & ~511);

			pBuffer += nFullContent + 512;
		}
		
		// we reach here -> we should be positioned to the buffer we care !
		pTarBlock = (struct posix_header*)((char *)pBuffer);
		nFileSize = getval8(pTarBlock->size, 12);
		checksum = 0;
		for(i = 0; i < 512; i++)
		{
			// the chksum positions are treated as blanks
			if ((i >= 148) && (i < 156))
			{
				checksum += 32;
			}
			else
			{
				checksum += 0xFF & pBuffer[i];
			}
		}
		if (checksum != getval8(pTarBlock->chksum, 8))
		{
			return FALSE;
		}

		// ok, it's a valid header, now return the content
		*pszFileBuffer = pBuffer + 512;
		nBufferSize = nFileSize;
		if ((pTarBlock->typeflag == '0') || (pTarBlock->typeflag == '\0'))
		{
			fIsFile = TRUE;
		}
		else
		{
			fIsFile = FALSE;
		}
		szFileName = CString(pTarBlock->name);
	}
	return TRUE;
}

CString CDecompressClass::GetErrorText()
{
	return m_szLastError;
}


