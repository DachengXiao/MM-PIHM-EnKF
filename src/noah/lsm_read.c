#include "pihm.h"

void ReadLsm (char *filename, double *latitude, double *longitude,
    ctrl_struct *ctrl, noahtbl_struct *noahtbl)
{
    int             i;
    FILE           *lsm_file;
    int             match;
    int             bytes_now;
    int             bytes_consumed = 0;
    char            cmdstr[MAXSTRING];
    char            buffer[MAXSTRING];

    /*
     * Open *.lsm file
     */
    lsm_file = fopen (filename, "r");
    CheckFile (lsm_file, filename);

    /*
     * Start reading lsm_file
     */
    FindLine (lsm_file, "BOF");
    NextLine (lsm_file, cmdstr);
    ReadKeywordDouble (cmdstr, "LATITUDE", latitude);

    NextLine (lsm_file, cmdstr);
    ReadKeywordDouble (cmdstr, "LONGITUDE", longitude);

    NextLine (lsm_file, cmdstr);
    ReadKeywordInt (cmdstr, "NSOIL", &ctrl->nsoil);
    if (ctrl->nsoil > MAXLYR - 1)
    {
        printf
            ("Error: the number of soil layers should not be larger than %d\n!",
            MAXLYR - 1);
        PihmExit (1);
    }

    NextLine (lsm_file, cmdstr);
    ReadKeywordStr (cmdstr, "SLDPTH_DATA", buffer);

    for (i = 0; i < ctrl->nsoil; i++)
    {
        match =
            sscanf (buffer + bytes_consumed, "%lf%n", &ctrl->sldpth[i],
            &bytes_now);
        if (match != 1)
        {
            printf ("ERROR: .lsm input format error!\n");
            PihmExit (1);
        }
        bytes_consumed += bytes_now;
    }

    NextLine (lsm_file, cmdstr);
    ReadKeywordInt (cmdstr, "RAD_MODE_DATA", &ctrl->rad_mode);

    NextLine (lsm_file, cmdstr);
    ReadKeywordDouble (cmdstr, "SBETA_DATA", &noahtbl->sbeta);

    NextLine (lsm_file, cmdstr);
    ReadKeywordDouble (cmdstr, "FXEXP_DATA", &noahtbl->fxexp);

    NextLine (lsm_file, cmdstr);
    ReadKeywordDouble (cmdstr, "CSOIL_DATA", &noahtbl->csoil);

    NextLine (lsm_file, cmdstr);
    ReadKeywordDouble (cmdstr, "SALP_DATA", &noahtbl->salp);

    NextLine (lsm_file, cmdstr);
    ReadKeywordDouble (cmdstr, "FRZK_DATA", &noahtbl->frzk);

    NextLine (lsm_file, cmdstr);
    ReadKeywordDouble (cmdstr, "ZBOT_DATA", &noahtbl->zbot);

    NextLine (lsm_file, cmdstr);
    ReadKeywordDouble (cmdstr, "TBOT_DATA", &noahtbl->tbot);

    NextLine (lsm_file, cmdstr);
    ReadKeywordDouble (cmdstr, "CZIL_DATA", &noahtbl->czil);

    NextLine (lsm_file, cmdstr);
    ReadKeywordDouble (cmdstr, "LVCOEF_DATA", &noahtbl->lvcoef);

    /* Output control */
    NextLine (lsm_file, cmdstr);
    ReadKeywordInt (cmdstr, "T1", &ctrl->prtvrbl[T1_CTRL]);

    NextLine (lsm_file, cmdstr);
    ReadKeywordInt (cmdstr, "STC", &ctrl->prtvrbl[STC_CTRL]);

    NextLine (lsm_file, cmdstr);
    ReadKeywordInt (cmdstr, "SMC", &ctrl->prtvrbl[SMC_CTRL]);

    NextLine (lsm_file, cmdstr);
    ReadKeywordInt (cmdstr, "SH2O", &ctrl->prtvrbl[SH2O_CTRL]);

    NextLine (lsm_file, cmdstr);
    ReadKeywordInt (cmdstr, "SNOWH", &ctrl->prtvrbl[SNOWH_CTRL]);

    NextLine (lsm_file, cmdstr);
    ReadKeywordInt (cmdstr, "ALBEDO", &ctrl->prtvrbl[ALBEDO_CTRL]);

    NextLine (lsm_file, cmdstr);
    ReadKeywordInt (cmdstr, "LE", &ctrl->prtvrbl[LE_CTRL]);

    NextLine (lsm_file, cmdstr);
    ReadKeywordInt (cmdstr, "SH", &ctrl->prtvrbl[SH_CTRL]);

    NextLine (lsm_file, cmdstr);
    ReadKeywordInt (cmdstr, "G", &ctrl->prtvrbl[G_CTRL]);

    NextLine (lsm_file, cmdstr);
    ReadKeywordInt (cmdstr, "ETP", &ctrl->prtvrbl[ETP_CTRL]);

    NextLine (lsm_file, cmdstr);
    ReadKeywordInt (cmdstr, "ESNOW", &ctrl->prtvrbl[ESNOW_CTRL]);

    NextLine (lsm_file, cmdstr);
    ReadKeywordInt (cmdstr, "ROOTW", &ctrl->prtvrbl[ROOTW_CTRL]);

    NextLine (lsm_file, cmdstr);
    ReadKeywordInt (cmdstr, "SOILM", &ctrl->prtvrbl[SOILM_CTRL]);

    NextLine (lsm_file, cmdstr);
    ReadKeywordInt (cmdstr, "SOLAR", &ctrl->prtvrbl[SOLAR_CTRL]);

    fclose (lsm_file);
}

void ReadRad (char *filename, forc_struct *forc)
{
    int             i, j;
    FILE           *rad_file;
    int             index;
    char            cmdstr[MAXSTRING];

    rad_file = fopen (filename, "r");
    CheckFile (rad_file, filename);

    FindLine (rad_file, "BOF");

    forc->nrad = CountOccurance (rad_file, "RAD_TS");

    if (forc->nrad != forc->nmeteo)
    {
        printf
            ("The number of radiation forcing time series should be the same as the number of meteorlogical forcing time series!\n");
        printf (".rad file format error!\n");
        PihmExit (1);
    }

    forc->rad = (tsdata_struct *)malloc (forc->nrad * sizeof (tsdata_struct));

    FindLine (rad_file, "BOF");

    NextLine (rad_file, cmdstr);
    for (i = 0; i < forc->nrad; i++)
    {
        ReadKeywordInt (cmdstr, "RAD_TS", &index);
        if (i != index - 1)
        {
            printf
                ("Cannot read information of the %dth forcing series!\n",
                i + 1);
            printf (".rad file format error!\n");
            PihmExit (1);
        }

        /* Skip header lines */
        NextLine (rad_file, cmdstr);
        NextLine (rad_file, cmdstr);
        forc->rad[i].length = CountLine (rad_file, cmdstr, 1, "RAD_TS");
    }

    /* Rewind and read */
    FindLine (rad_file, "BOF");
    for (i = 0; i < forc->nrad; i++)
    {
        /* Skip header lines */
        NextLine (rad_file, cmdstr);
        NextLine (rad_file, cmdstr);
        NextLine (rad_file, cmdstr);

        forc->rad[i].ftime =
            (int *)malloc (forc->rad[i].length * sizeof (int));
        forc->rad[i].data =
            (double **)malloc (forc->rad[i].length * sizeof (double *));
        for (j = 0; j < forc->rad[i].length; j++)
        {
            forc->rad[i].data[j] = (double *)malloc (2 * sizeof (double));
            NextLine (rad_file, cmdstr);
            ReadTS (cmdstr, &forc->rad[i].ftime[j],
                &forc->rad[i].data[j][0], 2);
        }
    }

    fclose (rad_file);
}
