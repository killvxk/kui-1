#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////
START_NAMESPACE_KCORE

#define VFS_NAME "memvfs"

typedef struct SQLITE_MEM_FILE_BUFFER
{
	char* data;
	int used;
	int total;
	CRITICAL_SECTION  cs;
}SQLITE_MEM_FILE_BUFFER;

typedef struct SQLITE_MEM_DB
{
	sqlite3 * handle;
	SQLITE_MEM_FILE_BUFFER * mem;
}SQLITE_MEM_DB;

int MemVFSEnvInit();
void MemVFSEnvFini();
int MemVFSOpenDb( SQLITE_MEM_DB * db, const char * path, SQLITE_MEM_FILE_BUFFER * mem );
int MemVFSCloseDb( SQLITE_MEM_DB * db );

//////////////////////////////////////////////////////////////////////////

#pragma warning(disable:4996)
#pragma warning(disable:4244)

#define MEMVFS_MIN(x,y) ((x)<(y)?(x):(y))
#define MEMVFS_MAX(x,y) ((x)>(y)?(x):(y))

typedef struct spmemfile_t {
	sqlite3_file base;
	char * path;
	int flags;
	SQLITE_MEM_FILE_BUFFER * mem;
} spmemfile_t;

static void MEMVFS_DEBUG(const char *format, ...);
static int memfileClose( sqlite3_file * file );
static int memfileRead( sqlite3_file * file, void * buffer, int len, sqlite3_int64 offset );
static int memfileWrite( sqlite3_file * file, const void * buffer, int len, sqlite3_int64 offset );
static int memfileTruncate( sqlite3_file * file, sqlite3_int64 size );
static int memfileSync( sqlite3_file * file, int flags );
static int memfileFileSize( sqlite3_file * file, sqlite3_int64 * size );
static int memfileLock( sqlite3_file * file, int type );
static int memfileUnlock( sqlite3_file * file, int type );
static int memfileCheckReservedLock( sqlite3_file * file, int * result );
static int memfileFileControl( sqlite3_file * file, int op, void * arg );
static int memfileSectorSize( sqlite3_file * file );
static int memfileDeviceCharacteristics( sqlite3_file * file );

static sqlite3_io_methods g_spmemfile_io_memthods = {
	1,                                  /* iVersion */
	memfileClose,                     /* xClose */
	memfileRead,                      /* xRead */
	memfileWrite,                     /* xWrite */
	memfileTruncate,                  /* xTruncate */
	memfileSync,                      /* xSync */
	memfileFileSize,                  /* xFileSize */
	memfileLock,                      /* xLock */
	memfileUnlock,                    /* xUnlock */
	memfileCheckReservedLock,         /* xCheckReservedLock */
	memfileFileControl,               /* xFileControl */
	memfileSectorSize,                /* xSectorSize */
	memfileDeviceCharacteristics      /* xDeviceCharacteristics */
};

void MEMVFS_DEBUG(const char *format, ...)
{

}

int memfileClose( sqlite3_file * file )
{
	spmemfile_t * memfile = (spmemfile_t*)file;

	MEMVFS_DEBUG( "call %s( %p )", __FUNCTION__, memfile );

	if( SQLITE_OPEN_MAIN_DB & memfile->flags ) {
		// noop
	} else {
		if( NULL != memfile->mem ) {
			if( memfile->mem->data ) free( memfile->mem->data );
			free( memfile->mem );
		}
	}

	free( memfile->path );

	return SQLITE_OK;
}

int memfileRead( sqlite3_file * file, void * buffer, int len, sqlite3_int64 offset )
{
	spmemfile_t * memfile = (spmemfile_t*)file;

	MEMVFS_DEBUG( "call %s( %p, ..., %d, %lld ), len %d",
		__FUNCTION__, memfile, len, offset, memfile->mem->used );

	if( ( offset + len ) > memfile->mem->used ) {
		return SQLITE_IOERR_SHORT_READ;
	}

	memcpy( buffer, memfile->mem->data + offset, len );

	return SQLITE_OK;
}

int memfileWrite( sqlite3_file * file, const void * buffer, int len, sqlite3_int64 offset )
{
	spmemfile_t * memfile = (spmemfile_t*)file;
	SQLITE_MEM_FILE_BUFFER * mem = memfile->mem;

	if( SQLITE_OPEN_MAIN_DB & memfile->flags ) {
		EnterCriticalSection( &mem->cs );
	}

	MEMVFS_DEBUG( "call %s( %p, ..., %d, %lld ), len %d, total %d",
		__FUNCTION__, memfile, len, offset, mem->used, mem->total );

	if( ( offset + len ) > mem->total ) {
		int newTotal = 2 * ( offset + len + mem->total );
		char * newBuffer = (char*)realloc( mem->data, newTotal );

		if( NULL == newBuffer ) {
			if( SQLITE_OPEN_MAIN_DB & memfile->flags ) {
				LeaveCriticalSection( &mem->cs );
			}
			return SQLITE_NOMEM;
		}

		mem->total = newTotal;
		mem->data = newBuffer;
	}

	memcpy( mem->data + offset, buffer, len );

	mem->used = MEMVFS_MAX( mem->used, offset + len );

	if( SQLITE_OPEN_MAIN_DB & memfile->flags ) {
		LeaveCriticalSection( &mem->cs );
	}

	return SQLITE_OK;
}

int memfileTruncate( sqlite3_file * file, sqlite3_int64 size )
{
	spmemfile_t * memfile = (spmemfile_t*)file;

	MEMVFS_DEBUG( "call %s( %p )", __FUNCTION__, memfile );

	memfile->mem->used = MEMVFS_MIN( memfile->mem->used, size );

	return SQLITE_OK;
}

int memfileSync( sqlite3_file * file, int flags )
{
	MEMVFS_DEBUG( "call %s( %p )", __FUNCTION__, file );

	return SQLITE_OK;
}

int memfileFileSize( sqlite3_file * file, sqlite3_int64 * size )
{
	spmemfile_t * memfile = (spmemfile_t*)file;

	MEMVFS_DEBUG( "call %s( %p )", __FUNCTION__, memfile );

	* size = memfile->mem->used;

	return SQLITE_OK;
}

int memfileLock( sqlite3_file * file, int type )
{
	MEMVFS_DEBUG( "call %s( %p )", __FUNCTION__, file );

	return SQLITE_OK;
}

int memfileUnlock( sqlite3_file * file, int type )
{
	MEMVFS_DEBUG( "call %s( %p )", __FUNCTION__, file );

	return SQLITE_OK;
}

int memfileCheckReservedLock( sqlite3_file * file, int * result )
{
	MEMVFS_DEBUG( "call %s( %p )", __FUNCTION__, file );

	*result = 0;

	return SQLITE_OK;
}

int memfileFileControl( sqlite3_file * file, int op, void * arg )
{
	MEMVFS_DEBUG( "call %s( %p )", __FUNCTION__, file );

	if (op == SQLITE_FCNTL_PRAGMA)
		return SQLITE_NOTFOUND;

	return SQLITE_OK;
}

int memfileSectorSize( sqlite3_file * file )
{
	MEMVFS_DEBUG( "call %s( %p )", __FUNCTION__, file );

	return 512;
}

int memfileDeviceCharacteristics( sqlite3_file * file )
{
	MEMVFS_DEBUG( "call %s( %p )", __FUNCTION__, file );

	return 0;
}

//===========================================================================

typedef struct memvfs_cb_t {
	void * arg;
	SQLITE_MEM_FILE_BUFFER * ( * load ) ( void * args, const char * path );
} memvfs_cb_t;

typedef struct memvfs_t {
	sqlite3_vfs base;
	memvfs_cb_t cb;
	sqlite3_vfs * parent;
} memvfs_t;

static int memvfsOpen( sqlite3_vfs * vfs, const char * path, sqlite3_file * file, int flags, int * outflags );
static int memvfsDelete( sqlite3_vfs * vfs, const char * path, int syncDir );
static int memvfsAccess( sqlite3_vfs * vfs, const char * path, int flags, int * result );
static int memvfsFullPathname( sqlite3_vfs * vfs, const char * path, int len, char * fullpath );
static void * memvfsDlOpen( sqlite3_vfs * vfs, const char * path );
static void memvfsDlError( sqlite3_vfs * vfs, int len, char * errmsg );
static void ( * memvfsDlSym ( sqlite3_vfs * vfs, void * handle, const char * symbol ) ) ( void );
static void memvfsDlClose( sqlite3_vfs * vfs, void * handle );
static int memvfsRandomness( sqlite3_vfs * vfs, int len, char * buffer );
static int memvfsSleep( sqlite3_vfs * vfs, int microseconds );
static int memvfsCurrentTime( sqlite3_vfs * vfs, double * result );

static memvfs_t g_memvfs = {
	{
		1,                                           /* iVersion */
			0,                                           /* szOsFile */
			0,                                           /* mxPathname */
			0,                                           /* pNext */
			VFS_NAME,                               /* zName */
			0,                                           /* pAppData */
			memvfsOpen,                                /* xOpen */
			memvfsDelete,                              /* xDelete */
			memvfsAccess,                              /* xAccess */
			memvfsFullPathname,                        /* xFullPathname */
			memvfsDlOpen,                              /* xDlOpen */
			memvfsDlError,                             /* xDlError */
			memvfsDlSym,                               /* xDlSym */
			memvfsDlClose,                             /* xDlClose */
			memvfsRandomness,                          /* xRandomness */
			memvfsSleep,                               /* xSleep */
			memvfsCurrentTime                          /* xCurrentTime */
	}, 
	{ 0 },
	0                                                /* pParent */
};

int memvfsOpen( sqlite3_vfs * vfs, const char * path, sqlite3_file * file, int flags, int * outflags )
{
	memvfs_t * memvfs = (memvfs_t*)vfs;
	spmemfile_t * memfile = (spmemfile_t*)file;

	MEMVFS_DEBUG( "call %s( %p(%p), %s, %p, %x, %p )\n",
		__FUNCTION__, vfs, &g_memvfs, path, file, flags, outflags );

	memset( memfile, 0, sizeof( spmemfile_t ) );
	memfile->base.pMethods = &g_spmemfile_io_memthods;
	memfile->flags = flags;

	memfile->path = _strdup( path );

	if( SQLITE_OPEN_MAIN_DB & memfile->flags ) {
		memfile->mem = memvfs->cb.load( memvfs->cb.arg, path );
	} else {
		memfile->mem = (SQLITE_MEM_FILE_BUFFER*)calloc( sizeof( SQLITE_MEM_FILE_BUFFER ), 1 );
	}

	return memfile->mem ? SQLITE_OK : SQLITE_ERROR;
}

int memvfsDelete( sqlite3_vfs * vfs, const char * path, int syncDir )
{
	MEMVFS_DEBUG( "call %s( %p(%p), %s, %d )\n",
		__FUNCTION__, vfs, &g_memvfs, path, syncDir );

	return SQLITE_OK;
}

int memvfsAccess( sqlite3_vfs * vfs, const char * path, int flags, int * result )
{
	return SQLITE_OK;
}

int memvfsFullPathname( sqlite3_vfs * vfs, const char * path, int len, char * fullpath )
{
	ZeroMemory(fullpath, sizeof(fullpath));
	strncpy( fullpath, path, len );
	fullpath[ len - 1 ] = '\0';

	return SQLITE_OK;
}

void * memvfsDlOpen( sqlite3_vfs * vfs, const char * path )
{
	return NULL;
}

void memvfsDlError( sqlite3_vfs * vfs, int len, char * errmsg )
{
	// noop
}

void ( * memvfsDlSym ( sqlite3_vfs * vfs, void * handle, const char * symbol ) ) ( void )
{
	return NULL;
}

void memvfsDlClose( sqlite3_vfs * vfs, void * handle )
{
	// noop
}

int memvfsRandomness( sqlite3_vfs * vfs, int len, char * buffer )
{
	return SQLITE_OK;
}

int memvfsSleep( sqlite3_vfs * vfs, int microseconds )
{
	return SQLITE_OK;
}

#include <time.h>
int memvfsCurrentTime( sqlite3_vfs * vfs, double * result )
{
	FILETIME ft;
	/* FILETIME structure is a 64-bit value representing the number of 
	100-nanosecond intervals since January 1, 1601 (= JD 2305813.5). 
	*/
	sqlite3_int64 timeW;   /* Whole days */
	sqlite3_int64 timeF;   /* Fractional Days */

	/* Number of 100-nanosecond intervals in a single day */
	static const sqlite3_int64 ntuPerDay = 
		10000000*(sqlite3_int64)86400;

	/* Number of 100-nanosecond intervals in half of a day */
	static const sqlite3_int64 ntuPerHalfDay = 
		10000000*(sqlite3_int64)43200;

	/* 2^32 - to avoid use of LL and warnings in gcc */
	static const sqlite3_int64 max32BitValue = 
		(sqlite3_int64)2000000000 + (sqlite3_int64)2000000000 + (sqlite3_int64)294967296;

	GetSystemTimeAsFileTime( &ft );

	timeW = (((sqlite3_int64)ft.dwHighDateTime)*max32BitValue) + (sqlite3_int64)ft.dwLowDateTime;
	timeF = timeW % ntuPerDay;          /* fractional days (100-nanoseconds) */
	timeW = timeW / ntuPerDay;          /* whole days */
	timeW = timeW + 2305813;            /* add whole days (from 2305813.5) */
	timeF = timeF + ntuPerHalfDay;      /* add half a day (from 2305813.5) */
	timeW = timeW + (timeF/ntuPerDay);  /* add whole day if half day made one */
	timeF = timeF % ntuPerDay;          /* compute new fractional days */
	*result = (double)timeW + ((double)timeF / (double)ntuPerDay);
	return SQLITE_OK;
}

//===========================================================================

int spmemvfs_init( memvfs_cb_t * cb )
{
	sqlite3_vfs * parent = NULL;

	if( g_memvfs.parent ) return SQLITE_OK;

	parent = sqlite3_vfs_find( 0 );

	g_memvfs.parent = parent;

	g_memvfs.base.mxPathname = parent->mxPathname;
	g_memvfs.base.szOsFile = sizeof( spmemfile_t );

	g_memvfs.cb = * cb;

	return sqlite3_vfs_register( (sqlite3_vfs*)&g_memvfs, 0 );
}

//===========================================================================

typedef struct spmembuffer_link_t {
	char * path;
	SQLITE_MEM_FILE_BUFFER * mem;
	struct spmembuffer_link_t * next;
} spmembuffer_link_t;

spmembuffer_link_t * spmembuffer_link_remove( spmembuffer_link_t ** head, const char * path )
{
	spmembuffer_link_t * ret = NULL;

	spmembuffer_link_t ** iter = head;
	for( ; NULL != *iter; ) {
		spmembuffer_link_t * curr = *iter;

		if( 0 == strcmp( path, curr->path ) ) {
			ret = curr;
			*iter = curr->next;
			break;
		} else {
			iter = &( curr->next );
		}
	}

	return ret;
}

void spmembuffer_link_free( spmembuffer_link_t * iter )
{
	free( iter->path );
	free( iter->mem->data );
	free( iter->mem );
	free( iter );
}

//===========================================================================

typedef struct spmemvfs_env_t {
	spmembuffer_link_t * head;
	sqlite3_mutex * mutex;
} spmemvfs_env_t;

static spmemvfs_env_t * g_spmemvfs_env = NULL;

static SQLITE_MEM_FILE_BUFFER * load_cb( void * arg, const char * path )
{
	SQLITE_MEM_FILE_BUFFER * ret = NULL;

	spmemvfs_env_t * env = (spmemvfs_env_t*)arg;

	sqlite3_mutex_enter( env->mutex );
	{
		spmembuffer_link_t * toFind = spmembuffer_link_remove( &( env->head ), path );

		if( NULL != toFind ) {
			ret = toFind->mem;
			free( toFind->path );
			free( toFind );
		}
	}
	sqlite3_mutex_leave( env->mutex );

	return ret;
}

int MemVFSEnvInit()
{
	int ret = 0;

	if( NULL == g_spmemvfs_env ) {
		memvfs_cb_t cb;

		g_spmemvfs_env = (spmemvfs_env_t*)calloc( sizeof( spmemvfs_env_t ), 1 );
		g_spmemvfs_env->mutex = sqlite3_mutex_alloc( SQLITE_MUTEX_FAST );

		cb.arg = g_spmemvfs_env;
		cb.load = load_cb;

		ret = spmemvfs_init( &cb );
	}

	return ret;
}

void MemVFSEnvFini()
{
	if( NULL != g_spmemvfs_env ) {
		spmembuffer_link_t * iter = NULL;

		sqlite3_vfs_unregister( (sqlite3_vfs*)&g_memvfs );

		sqlite3_mutex_free( g_spmemvfs_env->mutex );

		iter = g_spmemvfs_env->head;
		for( ; NULL != iter; ) {
			spmembuffer_link_t * next = iter->next;

			spmembuffer_link_free( iter );

			iter = next;
		}

		free( g_spmemvfs_env );
		g_spmemvfs_env = NULL;
		g_memvfs.parent = NULL;
	}
}

int MemVFSOpenDb( SQLITE_MEM_DB * db, const char * path, SQLITE_MEM_FILE_BUFFER * mem )
{
	int ret = 0;

	spmembuffer_link_t * iter = NULL;

	memset( db, 0, sizeof( SQLITE_MEM_DB ) );

	iter = (spmembuffer_link_t*)calloc( sizeof( spmembuffer_link_t ), 1 );
	iter->path = strdup( path );
	iter->mem = mem;

	sqlite3_mutex_enter( g_spmemvfs_env->mutex );
	{
		iter->next = g_spmemvfs_env->head;
		g_spmemvfs_env->head = iter;
	}
	sqlite3_mutex_leave( g_spmemvfs_env->mutex );

	ret = sqlite3_open_v2( path, &(db->handle),
		SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, VFS_NAME );

	if( 0 == ret ) {
		db->mem = mem;
	} else {
		sqlite3_mutex_enter( g_spmemvfs_env->mutex );
		{
			iter = spmembuffer_link_remove( &(g_spmemvfs_env->head), path );
			if( NULL != iter ) spmembuffer_link_free( iter );
		}
		sqlite3_mutex_leave( g_spmemvfs_env->mutex );
	}

	return ret;
}

int MemVFSCloseDb( SQLITE_MEM_DB * db )
{
	int ret = 0;

	if( NULL == db ) return 0;

	if( NULL != db->handle ) {
		ret = sqlite3_close( db->handle );
		db->handle = NULL;
	}

	if( NULL != db->mem ) {
		if( NULL != db->mem->data ) free( db->mem->data );
		free( db->mem );
		db->mem = NULL;
	}

	return ret;
}

typedef struct sqlitememory
{
	TCHAR  f_db_filename[MAX_PATH];
	SQLITE_MEM_DB f_mem_db;
	SQLITE_MEM_FILE_BUFFER* f_mem_db_buffer;
}sqlitememory;

bool __stdcall AfxOpenSqliteMemory(LPCTSTR szDbFileName, sqlite3*& pSqlite, DWORD_PTR& cookie)
{
	sqlitememory* pSqliteMemory = new sqlitememory;
	HANDLE hFile = CreateFile(szDbFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (pSqliteMemory == NULL || hFile == INVALID_HANDLE_VALUE)
	{
		if (pSqliteMemory != NULL)
			delete pSqliteMemory;
		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);
		return false;
	}

	ZeroMemory(pSqliteMemory, sizeof(sqlitememory));
	char szFileNameUtf8[MAX_PATH] = {0};
	int nSize =  _countof(szFileNameUtf8);
	kcore::AfxStr2UTF8(szDbFileName, szFileNameUtf8, nSize);
	lstrcpy(pSqliteMemory->f_db_filename, szDbFileName);	
	{
		MemVFSEnvInit();
		pSqliteMemory->f_mem_db_buffer = (SQLITE_MEM_FILE_BUFFER*)calloc( sizeof(SQLITE_MEM_FILE_BUFFER), 1 );
		InitializeCriticalSection (&pSqliteMemory->f_mem_db_buffer->cs);
		DWORD dwReadBytes = 0;
		DWORD dwSize = GetFileSize(hFile, NULL);
		pSqliteMemory->f_mem_db_buffer->total = pSqliteMemory->f_mem_db_buffer->used = dwSize;
		pSqliteMemory->f_mem_db_buffer->data = (char*)calloc( dwSize + 1, 1 );	
		ReadFile(hFile, pSqliteMemory->f_mem_db_buffer->data, dwSize, &dwReadBytes, NULL);
		pSqliteMemory->f_mem_db_buffer->data[dwSize] = '\0';
	}
	CloseHandle(hFile);

	bool bret = false;
	if (SQLITE_OK != MemVFSOpenDb(&pSqliteMemory->f_mem_db, szFileNameUtf8, pSqliteMemory->f_mem_db_buffer))
		return false;
	pSqlite = pSqliteMemory->f_mem_db.handle;
	cookie = reinterpret_cast<DWORD_PTR>(pSqliteMemory);
	return true;
}

void __stdcall AfxFlushSqliteMemory(DWORD_PTR cookie)
{
	sqlitememory* pSqliteMemory = reinterpret_cast<sqlitememory*>(cookie);
	if (pSqliteMemory == NULL)
		return ;

	TCHAR szTempFile[MAX_PATH] = {0}, szOldFile[MAX_PATH] = {0};
	_stprintf_s(szTempFile, TEXT("%s.tmp"), pSqliteMemory->f_db_filename);
	_stprintf_s(szOldFile, TEXT("%s.old"), pSqliteMemory->f_db_filename);
	EnterCriticalSection(&pSqliteMemory->f_mem_db_buffer->cs);
	{
		DWORD dwBytesWritten = 0;
		HANDLE hFile = CreateFile(szTempFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			WriteFile(hFile,  pSqliteMemory->f_mem_db_buffer->data, pSqliteMemory->f_mem_db_buffer->used, &dwBytesWritten, NULL);
			FlushFileBuffers(hFile);
			CloseHandle(hFile);
		}
	}
	MoveFileEx(pSqliteMemory->f_db_filename, szOldFile, MOVEFILE_REPLACE_EXISTING);
	DeleteFile(pSqliteMemory->f_db_filename);
	MoveFileEx(szTempFile, pSqliteMemory->f_db_filename, MOVEFILE_REPLACE_EXISTING);
	LeaveCriticalSection(&pSqliteMemory->f_mem_db_buffer->cs );
}

void __stdcall AfxCloseSqliteMemory(DWORD_PTR cookie)
{
	sqlitememory* pSqliteMemory = reinterpret_cast<sqlitememory*>(cookie);
	if (pSqliteMemory != NULL)
	{
		DeleteCriticalSection(&pSqliteMemory->f_mem_db_buffer->cs);
		MemVFSCloseDb(&pSqliteMemory->f_mem_db);
		MemVFSEnvFini();
		ZeroMemory(pSqliteMemory, sizeof(*pSqliteMemory));
		delete pSqliteMemory;
		pSqliteMemory = NULL;
	}
}

END_NAMESPACE_KCORE