#include <gtest/gtest.h>
#include "bendcore/database_manager.h"

using namespace bendcore::database;

TEST(MaterialDatabaseTest, GetInstance) {
    auto& db1 = MaterialDatabase::getInstance();
    auto& db2 = MaterialDatabase::getInstance();
    EXPECT_EQ(&db1, &db2);  // Singleton pattern
}

TEST(MaterialDatabaseTest, GetSteel) {
    auto& db = MaterialDatabase::getInstance();
    auto steel = db.getMaterial("steel_mild");
    EXPECT_TRUE(steel.has_value());
    EXPECT_EQ(steel->name, "Mild Steel");
}

TEST(MaterialDatabaseTest, GetAllMaterials) {
    auto& db = MaterialDatabase::getInstance();
    auto materials = db.getAllMaterials();
    EXPECT_GT(materials.size(), 0);
}

TEST(MaterialDatabaseTest, GetMaterialsByType) {
    auto& db = MaterialDatabase::getInstance();
    auto steels = db.getMaterialsByType("steel");
    EXPECT_GT(steels.size(), 0);
    EXPECT_EQ(steels[0].type, "steel");
}

TEST(MaterialDatabaseTest, GetMaterialsInStock) {
    auto& db = MaterialDatabase::getInstance();
    auto inStock = db.getMaterialsInStock();
    EXPECT_GT(inStock.size(), 0);
    for (const auto& mat : inStock) {
        EXPECT_TRUE(mat.inStock);
    }
}

TEST(MaterialDatabaseTest, AddCustomMaterial) {
    auto& db = MaterialDatabase::getInstance();
    Material custom{
        "custom_alloy", "Custom Alloy", "composite",
        3.5, 80, 300, 400, 20.0, 12.0, "Supplier X", true
    };
    db.addMaterial(custom);

    auto found = db.getMaterial("custom_alloy");
    EXPECT_TRUE(found.has_value());
    EXPECT_EQ(found->name, "Custom Alloy");
}

TEST(MachineDatabaseTest, GetInstance) {
    auto& db1 = MachineDatabase::getInstance();
    auto& db2 = MachineDatabase::getInstance();
    EXPECT_EQ(&db1, &db2);
}

TEST(MachineDatabaseTest, GetAllMachines) {
    auto& db = MachineDatabase::getInstance();
    auto machines = db.getAllMachines();
    EXPECT_GT(machines.size(), 0);
}

TEST(MachineDatabaseTest, GetAvailableMachines) {
    auto& db = MachineDatabase::getInstance();
    auto available = db.getAvailableMachines();
    EXPECT_GT(available.size(), 0);
    for (const auto& m : available) {
        EXPECT_TRUE(m.isAvailable);
    }
}

TEST(MachineDatabaseTest, GetMachinesByType) {
    auto& db = MachineDatabase::getInstance();
    auto benders = db.getMachinesByType("bender");
    EXPECT_GT(benders.size(), 0);
    EXPECT_EQ(benders[0].machineType, "bender");
}

TEST(MachineDatabaseTest, FindMachinesForCapabilities) {
    auto& db = MachineDatabase::getInstance();
    auto suitable = db.getMachinesForCapabilities(30.0, 100000.0, 80.0);
    EXPECT_GT(suitable.size(), 0);
}

TEST(TemplateDatabaseTest, GetInstance) {
    auto& db1 = TemplateDatabase::getInstance();
    auto& db2 = TemplateDatabase::getInstance();
    EXPECT_EQ(&db1, &db2);
}

TEST(TemplateDatabaseTest, GetAllTemplates) {
    auto& db = TemplateDatabase::getInstance();
    auto templates = db.getAllTemplates();
    EXPECT_GT(templates.size(), 0);
}

TEST(TemplateDatabaseTest, GetTemplatesByCategory) {
    auto& db = TemplateDatabase::getInstance();
    auto common = db.getTemplatesByCategory("common");
    EXPECT_GT(common.size(), 0);
    EXPECT_EQ(common[0].category, "common");
}

TEST(TemplateDatabaseTest, FindSimilarTemplates) {
    auto& db = TemplateDatabase::getInstance();
    auto similar = db.findSimilarTemplates(12.0, 1.5, 1);
    EXPECT_GT(similar.size(), 0);
}

TEST(TemplateDatabaseTest, AddCustomTemplate) {
    auto& db = TemplateDatabase::getInstance();
    PipeTemplate custom{
        "custom_t1", "Custom Assembly", "My custom design",
        14.0, 1.5, 300.0, "aluminum",
        {60.0, 120.0}, {20.0, 25.0}, {100.0, 200.0},
        180.0, "custom"
    };
    db.addTemplate(custom);

    auto found = db.getTemplate("custom_t1");
    EXPECT_TRUE(found.has_value());
    EXPECT_EQ(found->name, "Custom Assembly");
}

TEST(SupplierDatabaseTest, GetInstance) {
    auto& db1 = SupplierDatabase::getInstance();
    auto& db2 = SupplierDatabase::getInstance();
    EXPECT_EQ(&db1, &db2);
}

TEST(SupplierDatabaseTest, GetAllSuppliers) {
    auto& db = SupplierDatabase::getInstance();
    auto suppliers = db.getAllSuppliers();
    EXPECT_GT(suppliers.size(), 0);
}

TEST(SupplierDatabaseTest, GetSuppliersByType) {
    auto& db = SupplierDatabase::getInstance();
    auto material_suppliers = db.getSuppliersByType("material");
    EXPECT_GT(material_suppliers.size(), 0);
}

TEST(SupplierDatabaseTest, GetSuppliersBySpecialty) {
    auto& db = SupplierDatabase::getInstance();
    auto bending_services = db.getSuppliersBySpecialty("bending");
    EXPECT_GT(bending_services.size(), 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
