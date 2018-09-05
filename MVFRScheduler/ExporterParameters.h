/// \file ExporterParameters.h Parameters for the configuration exporter
#ifndef EXPORTERPARAMETERS_H_
#define EXPORTERPARAMETERS_H_

enum class ExporterEngineType {
    RPP_CONFIG, CPP_FILE
};

/// Parameters for the TwoStage algorithm
struct ExporterParameters {
    int variantID; ///< Variant for which the configuration file should be created and exported
    ExporterEngineType exportType; ///< The type of the exportation engine
};

#endif