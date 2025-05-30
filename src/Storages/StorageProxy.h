#pragma once

#include <Storages/IStorage.h>
#include <Storages/SelectQueryInfo.h>
#include <QueryPipeline/Pipe.h>


namespace DB
{

class StorageProxy : public IStorage
{
public:

    explicit StorageProxy(const StorageID & table_id_) : IStorage(table_id_) {}

    virtual StoragePtr getNested() const = 0;

    String getName() const override { return "StorageProxy"; }

    bool isRemote() const override { return getNested()->isRemote(); }
    bool isView() const override { return getNested()->isView(); }
    bool supportsSampling() const override { return getNested()->supportsSampling(); }
    bool supportsFinal() const override { return getNested()->supportsFinal(); }
    bool supportsPrewhere() const override { return getNested()->supportsPrewhere(); }
    bool supportsReplication() const override { return getNested()->supportsReplication(); }
    bool supportsParallelInsert() const override { return getNested()->supportsParallelInsert(); }
    bool supportsDeduplication() const override { return getNested()->supportsDeduplication(); }
    bool noPushingToViewsOnInserts() const override { return getNested()->noPushingToViewsOnInserts(); }
    bool hasEvenlyDistributedRead() const override { return getNested()->hasEvenlyDistributedRead(); }

    ColumnSizeByName getColumnSizes() const override { return getNested()->getColumnSizes(); }

    QueryProcessingStage::Enum getQueryProcessingStage(
        ContextPtr context,
        QueryProcessingStage::Enum to_stage,
        const StorageSnapshotPtr &,
        SelectQueryInfo & info) const override
    {
        const auto & nested_metadata = getNested()->getInMemoryMetadataPtr();
        return getNested()->getQueryProcessingStage(context, to_stage, getNested()->getStorageSnapshot(nested_metadata, context), info);
    }

    Pipe watch(
        const Names & column_names,
        const SelectQueryInfo & query_info,
        ContextPtr context,
        QueryProcessingStage::Enum & processed_stage,
        size_t max_block_size,
        size_t num_streams) override
    {
        return getNested()->watch(column_names, query_info, context, processed_stage, max_block_size, num_streams);
    }

    void read(
        QueryPlan & query_plan,
        const Names & column_names,
        const StorageSnapshotPtr & storage_snapshot,
        SelectQueryInfo & query_info,
        ContextPtr context,
        QueryProcessingStage::Enum processed_stage,
        size_t max_block_size,
        size_t num_streams) override
    {
        getNested()->read(query_plan, column_names, storage_snapshot, query_info, context, processed_stage, max_block_size, num_streams);
    }

    SinkToStoragePtr write(const ASTPtr & query, const StorageMetadataPtr & metadata_snapshot, ContextPtr context, bool async_insert) override
    {
        return getNested()->write(query, metadata_snapshot, context, async_insert);
    }

    void drop() override { getNested()->drop(); }

    void truncate(
        const ASTPtr & query,
        const StorageMetadataPtr & metadata_snapshot,
        ContextPtr context,
        TableExclusiveLockHolder & lock) override
    {
        getNested()->truncate(query, metadata_snapshot, context, lock);
    }

    void rename(const String & new_path_to_table_data, const StorageID & new_table_id) override
    {
        getNested()->rename(new_path_to_table_data, new_table_id);
        IStorage::renameInMemory(new_table_id);
    }

    void renameInMemory(const StorageID & new_table_id) override
    {
        getNested()->renameInMemory(new_table_id);
        IStorage::renameInMemory(new_table_id);
    }

    void alter(const AlterCommands & params, ContextPtr context, AlterLockHolder & alter_lock_holder) override
    {
        getNested()->alter(params, context, alter_lock_holder);
        IStorage::setInMemoryMetadata(getNested()->getInMemoryMetadata());
    }

    void checkAlterIsPossible(const AlterCommands & commands, ContextPtr context) const override
    {
        getNested()->checkAlterIsPossible(commands, context);
    }

    Pipe alterPartition(
            const StorageMetadataPtr & metadata_snapshot,
            const PartitionCommands & commands,
            ContextPtr context) override
    {
        return getNested()->alterPartition(metadata_snapshot, commands, context);
    }

    void checkAlterPartitionIsPossible(const PartitionCommands & commands, const StorageMetadataPtr & metadata_snapshot, const Settings & settings, ContextPtr context) const override
    {
        getNested()->checkAlterPartitionIsPossible(commands, metadata_snapshot, settings, context);
    }

    bool optimize(
            const ASTPtr & query,
            const StorageMetadataPtr & metadata_snapshot,
            const ASTPtr & partition,
            bool final,
            bool deduplicate,
            const Names & deduplicate_by_columns,
            bool cleanup,
            ContextPtr context) override
    {
        return getNested()->optimize(query, metadata_snapshot, partition, final, deduplicate, deduplicate_by_columns, cleanup, context);
    }

    void mutate(const MutationCommands & commands, ContextPtr context) override { getNested()->mutate(commands, context); }

    CancellationCode killMutation(const String & mutation_id) override { return getNested()->killMutation(mutation_id); }

    void startup() override { getNested()->startup(); }
    void shutdown(bool is_drop) override { getNested()->shutdown(is_drop); }
    void flushAndPrepareForShutdown() override { getNested()->flushAndPrepareForShutdown(); }

    ActionLock getActionLock(StorageActionBlockType action_type) override { return getNested()->getActionLock(action_type); }

    DataValidationTasksPtr getCheckTaskList(const CheckTaskFilter & check_task_filter, ContextPtr context) override
    {
        return getNested()->getCheckTaskList(check_task_filter, context);
    }

    std::optional<CheckResult> checkDataNext(DataValidationTasksPtr & check_task_list) override
    {
        return getNested()->checkDataNext(check_task_list);
    }

    void checkTableCanBeDropped([[ maybe_unused ]] ContextPtr query_context) const override { getNested()->checkTableCanBeDropped(query_context); }

    bool storesDataOnDisk() const override { return getNested()->storesDataOnDisk(); }
    Strings getDataPaths() const override { return getNested()->getDataPaths(); }
    StoragePolicyPtr getStoragePolicy() const override { return getNested()->getStoragePolicy(); }
    std::optional<UInt64> totalRows(ContextPtr query_context) const override { return getNested()->totalRows(query_context); }
    std::optional<UInt64> totalBytes(ContextPtr query_context) const override { return getNested()->totalBytes(query_context); }
    std::optional<UInt64> lifetimeRows() const override { return getNested()->lifetimeRows(); }
    std::optional<UInt64> lifetimeBytes() const override { return getNested()->lifetimeBytes(); }

};


}
