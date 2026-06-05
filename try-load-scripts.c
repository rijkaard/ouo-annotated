/*
 * try-load-scripts — attempt to load every compiled wombat script in a
 * directory using the wombat_compile pipeline, and report failures.
 *
 * Errors surface when load_article() returns NULL.
 */

#include "containerhandle.h"
#include "dat.h"
#include "main.h"
#include "region.h"
#include "skill.h"
#include "terrain.h"
#include "wombat.h"
#include "wombat_compile.h"

#include <dirent.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Globals defined in main.c; provided here so try-load-scripts links without it. */
uint32_t              g_AutoInitialSpawnDeadline;
uint32_t              g_GameTick;
uint32_t              g_TimingBuffer[128];
uint32_t              g_TimingBufIndex;
uint32_t              g_TimingField9EC;
uint32_t              g_TimingField9F0;
uint32_t              g_LastTickElapsed;
uint32_t              g_TimingTotalTime;
uint32_t              g_TimingTickCount;
uint32_t              g_TimingPeakTime;
char                  g_LVNDestBuf[256];
HueEntry             *g_HueData;
HueEntryExpanded     *g_HueDataExp;
AnimDataEntry        *g_AnimData;
int                   g_HueDataLoaded;
int                   g_AnimDataLoaded;
int                   g_ArtLoaded;
int                   g_TileDataLoaded;
uint32_t              g_LoadingComplete;
volatile sig_atomic_t g_shutdown;
QuadCurve             g_StatCurve;
QuadCurve             g_SkillCurve;
double                g_StatCurveBase;
double                g_SkillCurveBase;
uint32_t              g_PoolBaseField_C4;
uint32_t              g_PoolBaseField_CC;
uint32_t              g_PoolBaseField_E4;
uint32_t              g_PoolBaseField_EC;
uint32_t              g_PoolBaseField_FC;
uint32_t              g_PoolBaseItems;
uint32_t              g_TileDataFileOffset;
uint32_t              g_HueFileOffset;

void ProgressBar_Update(int percent) { (void)percent; }
void InitPoolSizes(void) {}

/*
 * Load compiled wombat bytecode. Returns the parsed CScript on success,
 * or NULL on failure. Thin wrapper that keeps naming consistent with the
 * rest of the wombat_compile pipeline.
 */
static CScript *wombat_compile(const char *bytecode, const char *name)
{
    return ParseScriptOuter(bytecode, name);
}

/*
 * Read the compiled script at `path` and try to load it as `name`.
 * Returns the CScript on success, NULL on any failure.
 * On failure, cleans up the global compiler stack so the next call
 * starts from a fresh state.
 */
static CScript *load_article(const char *path, const char *name)
{
    FILE *fp = fopen(path, "rb");
    if (!fp) { perror(path); return NULL; }

    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (sz < 0) { fclose(fp); return NULL; }

    char *bytecode = malloc((size_t)sz + 1);
    if (!bytecode) { fclose(fp); return NULL; }
    fread(bytecode, 1, (size_t)sz, fp);
    bytecode[sz] = '\0';
    fclose(fp);

    CScript *script = wombat_compile(bytecode, name);
    free(bytecode);

    /* ParseScriptInner leaves g_ScriptCompiler on the stack on failure;
     * clean it up so the next load_article call starts fresh. */
    if (script == NULL && g_ScriptCompiler != NULL) {
        CScriptCompiler *stale = g_ScriptCompiler;
        CScriptCompiler_Destructor(stale);
        OperatorDelete(stale);
    }

    return script;
}

static void usage(const char *prog)
{
    fprintf(stderr,
            "Usage: %s [-sdb path] <scripts-directory>\n"
            "  -sdb <path>  SDB file (default: <directory>/sdb.txt)\n",
            prog);
}

int main(int argc, char *argv[])
{
    const char *sdb_path = NULL;
    const char *dir      = NULL;
    int i;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-sdb") == 0 && i + 1 < argc) {
            sdb_path = argv[++i];
        } else if (argv[i][0] != '-') {
            dir = argv[i];
        } else {
            fprintf(stderr, "unknown option: %s\n", argv[i]);
            usage(argv[0]);
            return 1;
        }
    }

    if (!dir) { usage(argv[0]); return 1; }

    /* Default sdb.txt to <directory>/sdb.txt */
    char default_sdb[4096];
    if (!sdb_path) {
        snprintf(default_sdb, sizeof(default_sdb), "%s/sdb.txt", dir);
        sdb_path = default_sdb;
    }

    /* Bootstrap the handle-map so _ServerSide I/O wrappers don't crash. */
    ContainerHandle_InitMap();

    /* Load the SDB; ParseScriptOuter needs it to resolve string identifiers. */
    if (CScriptStringDB_Load(&g_ScriptStringDB, sdb_path) != 0)
        fprintf(stderr, "warning: could not load SDB from '%s'\n", sdb_path);
    g_SdbLoaded = 1;

    DIR *d = opendir(dir);
    if (!d) { perror(dir); return 1; }

    int n_ok = 0, n_fail = 0;
    struct dirent *ent;
    char path[4096];
    char name[256];

    while ((ent = readdir(d)) != NULL) {
        const char *fname = ent->d_name;
        size_t len = strlen(fname);
        /* Ignore irrelevant source — only process .m files */
        if (len < 3 || fname[len - 2] != '.' || fname[len - 1] != 'm')
            continue;

        snprintf(path, sizeof(path), "%s/%s", dir, fname);

        /* Script name is filename without the .m extension */
        size_t nlen = len - 2;
        if (nlen >= sizeof(name)) nlen = sizeof(name) - 1;
        memcpy(name, fname, nlen);
        name[nlen] = '\0';

        if (!load_article(path, name)) {
            fprintf(stderr, "FAIL  %s\n", fname);
            n_fail++;
        } else {
            n_ok++;
        }
    }
    closedir(d);

    fprintf(stderr, "\n%d loaded, %d failed\n", n_ok, n_fail);
    return n_fail > 0 ? 1 : 0;
}
