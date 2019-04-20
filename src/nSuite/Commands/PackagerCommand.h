#pragma once
#ifndef PACKAGERCOMMAND_H
#define PACKAGERCOMMAND_H

#include "Command.h"


/** Command to compress an entire directory into a portable installer. */
class PackagerCommand : public Command {
public:
	// Public interface implementation
	virtual void execute(const int & argc, char * argv[]) const override;
};

#endif // PACKAGERCOMMAND_H