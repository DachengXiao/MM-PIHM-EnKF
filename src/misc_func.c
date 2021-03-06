#include "pihm.h"

void CreateOutputDir (char *outputdir, int spec_output_mode)
{
    time_t          rawtime;
    struct tm      *timestamp;
    char            str[11];

    if (0 == (mkdir ("output", 0755)))
    {
        printf (" Output directory was created.\n\n");
    }

    if (!spec_output_mode)
    {
        time (&rawtime);
        timestamp = localtime (&rawtime);

        /* Create output directory based on projectname and time */
        sprintf (str, "%2.2d%2.2d%2.2d%2.2d%2.2d",
            timestamp->tm_year + 1900 - 2000, timestamp->tm_mon + 1,
            timestamp->tm_mday, timestamp->tm_hour, timestamp->tm_min);
        sprintf (outputdir, "output/%s.%s/", project, str);

        printf ("\nOutput directory: %s\n", outputdir);
    }

    mkdir (outputdir, 0755);
}

void BKInput (char *simulation, char *outputdir)
{
    char            project[MAXSTRING];
    char           *token;
    char            tempname[MAXSTRING];
    char            system_cmd[MAXSTRING];
    char            source_file[MAXSTRING];

    strcpy (tempname, simulation);
    if (strstr (tempname, ".") != 0)
    {
        token = strtok (tempname, ".");
        strcpy (project, token);
    }
    else
    {
        strcpy (project, simulation);
    }

    /* Save input files into output directory */
    sprintf (source_file, "input/%s/%s.para", project, project);
    if (access (source_file, F_OK) != -1)
    {
        sprintf (system_cmd, "cp %s ./%s/%s.para.bak", source_file, outputdir,
            project);
        system (system_cmd);
    }
    sprintf (source_file, "input/%s/%s.calib", project, simulation);
    if (access (source_file, F_OK) != -1)
    {
        sprintf (system_cmd, "cp %s ./%s/%s.calib.bak", source_file,
            outputdir, simulation);
        system (system_cmd);
    }
    sprintf (source_file, "input/%s/%s.init", project, simulation);
    if (access (source_file, F_OK) != -1)
    {
        sprintf (system_cmd, "cp %s ./%s/%s.init.bak", source_file, outputdir,
            simulation);
        system (system_cmd);
    }
}

void PihmExit (int error)
{
#ifdef _ENKF_
    int             id;
    int             ierr;

    ierr = MPI_Comm_rank (MPI_COMM_WORLD, &id);
    printf ("Exit from Node %d\n", id);
    MPI_Abort (MPI_COMM_WORLD, error);
#else
    exit (error);
#endif
}
