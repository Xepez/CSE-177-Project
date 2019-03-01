#include "QueryCompiler.h"
#include "QueryOptimizer.h"
#include "Schema.h"
#include "ParseTree.h"
#include "Record.h"
#include "DBFile.h"
#include "Comparison.h"
#include "Function.h"
#include "RelOp.h"

using namespace std;


QueryCompiler::QueryCompiler(Catalog& _catalog, QueryOptimizer& _optimizer) : catalog(&_catalog), optimizer(&_optimizer) {
    
    catalog = &_catalog;
    optimizer = &_optimizer;
}

QueryCompiler::~QueryCompiler() {
}

void QueryCompiler::Compile(TableList* _tables, NameList* _attsToSelect, FuncOperator* _finalFunction, AndList* _predicate, NameList* _groupingAtts, int& _distinctAtts, QueryExecutionTree& _queryTree) {

    
	// create a SCAN operator for each table in the query
    TableList* scanTable = _tables;
    while (scanTable->tableName != NULL) {
        Schema scanSchema;
        DBFile db;
        string tableName = scanTable->tableName;
        
        if (catalog->GetSchema(tableName, scanSchema)) {
            // TODO: Ensure loading into different scans
            Scan scan(scanSchema, db);
        }
        else {
            cout << "ERROR GETTING SCHEMA IN SCAN - QueryCompiler.cc";
        }
        
        scanTable = scanTable->next;
    }
    

	// push-down selections: create a SELECT operator wherever necessary
    AndList* currAnd = _predicate;
    while(currAnd->left != NULL) {
        Schema selectSchema;
        Record selectConst;
        CNF selectPredicate;
        RelationalOp* selectProducer;
        
        // Finds schema used
//        TableList* selectTable = _tables;
//        while (selectTable->tableName != NULL) {
//            if (catalog->GetSchema(tableName, scanSchema)) {
//                if (scanSchema.Index(currAnd->left->left->value) != -1) {
//                    break;
//                }
//                selectSchema = NULL;
//            }
//        }
        
        // TODO
        AndList temp; // temp variable to get code to compile for now
        
        if (selectPredicate.ExtractCNF(*currAnd, selectSchema, selectConst) != 0) {
            cout << "ERROR GETTING CNF IN SELECT - QueryCompiler.cc";
        }
        
        // TODO
        Select select(selectSchema, selectPredicate, selectConst, selectProducer);
        
        currAnd = currAnd->rightAnd;
    }
    

	// call the optimizer to compute the join order
	OptimizationTree* root;
	optimizer->Optimize(_tables, _predicate, root);

	// create join operators based on the optimal order computed by the optimizer
    // TODO
    
    
    // create the remaining operators based on the query {
    // PROJECT
    NameList* projectSelect = _attsToSelect;
    while (projectSelect->name != NULL) {
        Schema projectSchemaIn, projectSchemaOut;
        int numAttsInput, numAttsOutput;
        int* keepMe;
        RelationalOp* projectProducer;
        
        // TODO
        Project(projectSchemaIn, projectSchemaOut, numAttsInput, numAttsOutput, keepMe, projectProducer);
        
        projectSelect = projectSelect->next;
    }
    
    // DISTINCT
    if (_distinctAtts == 1) {
        Schema distinctSchema;
        RelationalOp* distinctProducer;
        
        // TODO
        DuplicateRemoval distinct(distinctSchema, distinctProducer);
    }
    
    // SUM
    if (_finalFunction != NULL) {
        Schema sumSchemaIn, sumSchemaOut;
        RelationalOp* sumProducer;
        Function sumCompute;
        // TODO
        sumCompute.GrowFromParseTree(_finalFunction, sumSchemaIn);
        
        Sum sum(sumSchemaIn, sumSchemaOut, sumCompute, sumProducer);
    }
    
    // GROUP BY
    if (_groupingAtts != NULL) {
        Schema groupSchema, schemaIn, schemaOut;
        string groupName = _groupingAtts->name;
        int* groupAtts;
        int noGroupAtts = 0;
        RelationalOp* groupByProducer;
        Function compute;
        OrderMaker om;
        
        // Get schema and make OrderMaker
        if (catalog->GetSchema(groupName, groupSchema)) {
            // TODO
            OrderMaker om(groupSchema, groupAtts, noGroupAtts);
        }
        else {
            cout << "ERROR GETTING SCHEMA IN GROUP BY - QueryCompiler.cc";
        }
        
        // TODO
        GroupBy groupBy(schemaIn, schemaOut, om, compute, groupByProducer);
    }

    // WRITE OUT
    // TODO
    Schema schemaWO;
    string outFile;
    RelationalOp* woProducer;
    WriteOut writeOut(schemaWO, outFile, woProducer);
    // }
    
    
	// connect everything in the query execution tree and return

	// free the memory occupied by the parse tree since it is not necessary anymore
}
