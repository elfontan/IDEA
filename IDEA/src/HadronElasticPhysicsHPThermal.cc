//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: HadronElasticPhysicsHPThermal.cc 71037 2013-06-10 09:20:54Z gcosmo $
//
//---------------------------------------------------------------------------
//
// ClassName:   HadronElasticPhysicsHPThermal
//
// Author: 3 June 2010 V. Ivanchenko
//
// Modified:
// 03.06.2011 V.Ivanchenko change design - now first default constructor 
//            is called, HP model and cross section are added on top
//
//----------------------------------------------------------------------------
//
// HP model for n with E < 20 MeV

#include "HadronElasticPhysicsHPThermal.hh"
#include "G4SystemOfUnits.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4Neutron.hh"
#include "G4HadronicProcess.hh"
#include "G4HadronElastic.hh"
#include "G4NeutronHPElastic.hh"
#include "G4NeutronHPElasticData.hh"
#include "G4NeutronHPThermalScattering.hh"
#include "G4NeutronHPThermalScatteringData.hh"

// factory
#include "G4PhysicsConstructorFactory.hh"
//
G4_DECLARE_PHYSCONSTR_FACTORY(HadronElasticPhysicsHPThermal);

G4ThreadLocal G4bool HadronElasticPhysicsHPThermal::wasActivated = false;
G4ThreadLocal G4HadronElasticPhysics* HadronElasticPhysicsHPThermal::mainElasticBuilder = 0;

HadronElasticPhysicsHPThermal::HadronElasticPhysicsHPThermal(const G4bool withThermal, G4int ver)
  : G4VPhysicsConstructor("hElasticWEL_CHIPS_HP"), verbose(ver), wThermal(withThermal)
{
  if(verbose > 1) { 
    G4cout << "### HadronElasticPhysicsHPThermal: " << GetPhysicsName() 
	   << G4endl; 
  }
  mainElasticBuilder = new G4HadronElasticPhysics(verbose);
}

HadronElasticPhysicsHPThermal::~HadronElasticPhysicsHPThermal()
{
  delete mainElasticBuilder;
}

void HadronElasticPhysicsHPThermal::ConstructParticle()
{
  // G4cout << "G4HadronElasticPhysics::ConstructParticle" << G4endl;
  mainElasticBuilder->ConstructParticle();
}

void HadronElasticPhysicsHPThermal::ConstructProcess()
{
  if(wasActivated) { return; }
  wasActivated = true;
  //Needed because this is a TLS object and this method is called by all threads
  if ( ! mainElasticBuilder ) mainElasticBuilder = new G4HadronElasticPhysics(verbose);
  mainElasticBuilder->ConstructProcess();

  mainElasticBuilder->GetNeutronModel()->SetMinEnergy(19.5*MeV);

  G4HadronicProcess* hel = mainElasticBuilder->GetNeutronProcess();
  G4NeutronHPElastic* hp = new G4NeutronHPElastic();
  hel->RegisterMe(hp);
  hel->AddDataSet(new G4NeutronHPElasticData());

  if(wThermal) {
    hp->SetMinEnergy(4.*eV);
    G4NeutronHPThermalScattering *thermal = new G4NeutronHPThermalScattering;
    thermal->SetMaxEnergy(4.*eV);
    hel->RegisterMe(thermal);
    G4NeutronHPThermalScatteringData *thermaldata = new G4NeutronHPThermalScatteringData;
    hel->AddDataSet(thermaldata);
  }

  if(verbose > 1) {
    G4cout << "### HadronElasticPhysicsHPThermal is constructed " 
	   << G4endl;
  }
}


