/*
 * Note: this file originally auto-generated by mib2c using
 *  : mib2c.iterate.conf 15999 2007-03-25 22:32:02Z dts12 $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <fcntl.h>
#include <pcre.h>
#include "log.h"
#include "util.h"
#include "command.h"
#include "main.h"
#include "raidSetTable.h"

static struct timeval cache_timestamp = { 0, 0 };
static int last_index_used = 0;

#define MAX_CACHE_AGE 10
#define OVECCOUNT 90

/** Initializes the raidSetTable module */
void
init_raidSetTable(void)
{
  /* here we initialize all the tables we're planning on supporting */
    initialize_table_raidSetTable();
}

/** Initialize the raidSetTable table by defining its contents and how it's structured */
void
initialize_table_raidSetTable(void)
{
    static oid raidSetTable_oid[] = {1,3,6,1,4,1,20038,2,1,6,2};
    size_t raidSetTable_oid_len   = OID_LENGTH(raidSetTable_oid);
    netsnmp_handler_registration    *reg;
    netsnmp_iterator_info           *iinfo;
    netsnmp_table_registration_info *table_info;

    reg = netsnmp_create_handler_registration(
              "raidSetTable",     raidSetTable_handler,
              raidSetTable_oid, raidSetTable_oid_len,
              HANDLER_CAN_RONLY
              );

    table_info = SNMP_MALLOC_TYPEDEF( netsnmp_table_registration_info );
    netsnmp_table_helper_add_indexes(table_info,
                           ASN_INTEGER,  /* index: raidSetIndex */
                           0);
    table_info->min_column = COLUMN_RAIDSETNAME;
    table_info->max_column = COLUMN_RAIDSETCOMMENTS;
    
    iinfo = SNMP_MALLOC_TYPEDEF( netsnmp_iterator_info );
    iinfo->get_first_data_point = raidSetTable_get_first_data_point;
    iinfo->get_next_data_point  = raidSetTable_get_next_data_point;
    iinfo->table_reginfo        = table_info;
    
    netsnmp_register_table_iterator( reg, iinfo );

    /* Initialise the contents of the table here */
}

void update_setlist();
struct raidSetTable_entry  *raidSetTable_head;

struct raidSetTable_entry* raidSetTable_get_head()
{ return raidSetTable_head; }

/* create a new row in the (unsorted) table */
struct raidSetTable_entry *
raidSetTable_createEntry(long  raidSetIndex) 
{
    struct raidSetTable_entry *entry;

    entry = SNMP_MALLOC_TYPEDEF(struct raidSetTable_entry);
    if (!entry)
        return NULL;

    entry->raidSetIndex = raidSetIndex;
    entry->next = raidSetTable_head;
    raidSetTable_head = entry;
    return entry;
}

/* remove a row from the table */
void
raidSetTable_removeEntry( struct raidSetTable_entry *entry ) {
    struct raidSetTable_entry *ptr, *prev;

    if (!entry)
        return;    /* Nothing to remove */

    for ( ptr  = raidSetTable_head, prev = NULL;
          ptr != NULL;
          prev = ptr, ptr = ptr->next ) {
        if ( ptr == entry )
            break;
    }
    if ( !ptr )
        return;    /* Can't find it */

    if ( prev == NULL )
        raidSetTable_head = ptr->next;
    else
      prev->next = ptr->next;

    if (entry->raidSetName) free (entry->raidSetName);
    if (entry->raidSetType) free (entry->raidSetType);
    if (entry->raidSetComments) free (entry->raidSetComments);

    SNMP_FREE( entry );   /* XXX - release any other internal resources */
}


/* Example iterator hook routines - using 'get_next' to do most of the work */
netsnmp_variable_list *
raidSetTable_get_first_data_point(void **my_loop_context,
                          void **my_data_context,
                          netsnmp_variable_list *put_index_data,
                          netsnmp_iterator_info *mydata)
{
    *my_loop_context = raidSetTable_head;
    return raidSetTable_get_next_data_point(my_loop_context, my_data_context,
                                    put_index_data,  mydata );
}

netsnmp_variable_list *
raidSetTable_get_next_data_point(void **my_loop_context,
                          void **my_data_context,
                          netsnmp_variable_list *put_index_data,
                          netsnmp_iterator_info *mydata)
{
    struct raidSetTable_entry *entry = (struct raidSetTable_entry *)*my_loop_context;
    netsnmp_variable_list *idx = put_index_data;

    if ( entry ) {
        snmp_set_var_typed_integer( idx, ASN_INTEGER, entry->raidSetIndex );
        idx = idx->next_variable;
        *my_data_context = (void *)entry;
        *my_loop_context = (void *)entry->next;
        return put_index_data;
    } else {
        return NULL;
    }
}

/* Set List Updating */

void update_setlist()
{
  /* Calls 'raidutil list raidsetinfo' */
  struct timeval now;
  gettimeofday (&now, NULL);
  char *data = NULL;
  size_t data_len = 0;
  if (xsan_debug())
  {
    /* Use example data */
    int fd = open ("../examples/raidutil_list_raidsetinfo.txt", O_RDONLY);
    data = malloc (65536);
    data_len = read (fd, data, 65535);
    data[data_len] = '\0';
    close (fd);
  }
  else
  {
    /* Use live data */
    data = x_command_run("raidutil list raidsetinfo", 0);
    if (!data) return;
    data_len = strlen(data);
  }

  printf ("Data is '%s'\n", data);

  /* Regex and loop through each raid set row */
  const char *error;
  int erroffset;
  int ovector[OVECCOUNT];
  /* Line is like 'Raid5Set      RAID 5     2,3,4      3.92TB    0.00MB  No tasks running' */
  pcre *re = pcre_compile("^(\\w[^ ]+)\\s+(.*)\\s+((?:\\d\\,?)+)\\s+(\\d+\\.\\d\\d)([TGM])B\\s+(\\d+\\.\\d\\d)([TGM])B\\s+(.*)$", PCRE_MULTILINE, &error, &erroffset, NULL);
      
  if (re == NULL) { x_printf ("ERROR: update_setlist failed to compile regex"); free (data); return; }
  
  ovector[0] = 0;
  ovector[1] = 0;
  while(1)
  {
    int options = 0;                 /* Normally no options */
    int start_offset = ovector[1];   /* Start at end of previous match */
  
    if (ovector[0] == ovector[1])
    {
      if (ovector[0] == (int)data_len) break;
    }

    int rc = pcre_exec( re,                   /* the compiled pattern */
                         NULL,                 /* no extra data - we didn't study the pattern */
                         data,              /* the subject string */
                         data_len,       /* the length of the subject */
                         start_offset,         /* starting offset in the subject */
                         options,              /* options */
                         ovector,              /* output vector for substring information */
                         OVECCOUNT);           /* number of elements in the output vector */
                         
    if (rc == PCRE_ERROR_NOMATCH)
    {
      x_printf ("update_setlist no match found for regex");
      if (options == 0) break;
      ovector[1] = start_offset + 1;
      continue;    /* Go round the loop again */
    }
    
    /* Other matching errors are not recoverable. */
    if (rc > 0)
    {
      /* Matched an RAID Set. Vectors:
       * 0=FullString 1(2)=Name 2(4)=Type 3(6)=Drives 4(8)=Size 5(10)=SizeUnits 6(12)=Unallocated 7(14)=UnallocatedUnits 8(16)=Comments
      */
    
      /* Get Volume Name */
      char *name_str;
      asprintf (&name_str, "%.*s", ovector[3] - ovector[2], data + ovector[2]);
      trim_end(name_str);
      x_debug ("update_setlist Matched %.*s\n", ovector[3] - ovector[2], data + ovector[2]);
  
      /* Find/Create Volume Entry */ 
      struct raidSetTable_entry *entry = raidSetTable_head;
      while (entry)
      {
        if (!strcmp(name_str, entry->raidSetName)) break;
        entry = entry->next;
      }
      if (!entry)
      { 
        last_index_used++;
        entry = raidSetTable_createEntry(last_index_used);
        entry->raidSetName = strdup(name_str);
        entry->raidSetName_len = strlen (entry->raidSetName);
      }
      entry->last_seen = now.tv_sec;           
      free (name_str);
      name_str = NULL;
       
      /* Extract Data from Regex Match */
      extract_string_in_range(data+ovector[4], ovector[5]-ovector[4], &entry->raidSetType, &entry->raidSetType_len);
      entry->raidSetSize = extract_uint_in_range(data+ovector[8], ovector[9]-ovector[8]);
      scale_value_to_m(data[ovector[10]], &entry->raidSetSize);
      entry->raidSetUnused = extract_uint_in_range(data+ovector[12], ovector[13]-ovector[12]);
      scale_value_to_m(data[ovector[14]], &entry->raidSetUnused);
      extract_string_in_range(data+ovector[16], ovector[17]-ovector[16], &entry->raidSetComments, &entry->raidSetComments_len);
    }
    else
    {
      pcre_free(re);    /* Release memory used for the compiled pattern */
      return;
    }
  }

  pcre_free(re);    /* Release memory used for the compiled pattern */

  /* Check for obsolete entries */
  struct raidSetTable_entry *entry = raidSetTable_head;
  while (entry)
  {
    struct raidSetTable_entry *next = entry->next;
    if (entry->last_seen != now.tv_sec)
    {
      /* Entry is obsolete */
      raidSetTable_removeEntry(entry);
    }
    entry = next;
  }

  /* Update cache timestamp */
  gettimeofday(&cache_timestamp, NULL);

  /* Clean up */
  free (data);
  data = NULL;
  data_len = 0;
}

void update_setlist_if_necessary()
{
  struct timeval now;
  gettimeofday(&now, NULL);
  if (now.tv_sec - cache_timestamp.tv_sec > MAX_CACHE_AGE)
  { update_setlist(); }
}

/** handles requests for the raidSetTable table */
int
raidSetTable_handler(
    netsnmp_mib_handler               *handler,
    netsnmp_handler_registration      *reginfo,
    netsnmp_agent_request_info        *reqinfo,
    netsnmp_request_info              *requests) {

    netsnmp_request_info       *request;
    netsnmp_table_request_info *table_info;
    struct raidSetTable_entry          *table_entry;

    switch (reqinfo->mode) {
        /*
         * Read-support (also covers GetNext requests)
         */
    case MODE_GET:
        update_setlist_if_necessary();
        for (request=requests; request; request=request->next) {
            table_entry = (struct raidSetTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_RAIDSETNAME:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_value( request->requestvb, ASN_OCTET_STR,
                                 (u_char*)table_entry->raidSetName,
                                          table_entry->raidSetName_len);
                break;
            case COLUMN_RAIDSETTYPE:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_value( request->requestvb, ASN_OCTET_STR,
                                 (u_char*)table_entry->raidSetType,
                                          table_entry->raidSetType_len);
                break;
            case COLUMN_RAIDSETSIZE:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_GAUGE,
                                            table_entry->raidSetSize);
                break;
            case COLUMN_RAIDSETUNUSED:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_GAUGE,
                                            table_entry->raidSetUnused);
                break;
            case COLUMN_RAIDSETCOMMENTS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_value( request->requestvb, ASN_OCTET_STR,
                                 (u_char*)table_entry->raidSetComments,
                                          table_entry->raidSetComments_len);
                break;
            default:
                netsnmp_set_request_error(reqinfo, request,
                                          SNMP_NOSUCHOBJECT);
                break;
            }
        }
        break;

    }
    return SNMP_ERR_NOERROR;
}
