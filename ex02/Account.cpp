/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Account.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/21 12:16:12 by vberdugo          #+#    #+#             */
/*   Updated: 2025/06/21 12:21:39 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Account.hpp"
#include <iostream>
#include <iomanip>
#include <ctime>

int Account::_nbAccounts = 0;
int Account::_totalAmount = 0;
int Account::_totalNbDeposits = 0;
int Account::_totalNbWithdrawals = 0;

void Account::_displayTimestamp() {
    std::time_t now = std::time(NULL);
    std::tm* lt = std::localtime(&now);
    std::cout << '[' << (1900 + lt->tm_year)
              << std::setw(2) << std::setfill('0') << (lt->tm_mon + 1)
              << std::setw(2) << lt->tm_mday << '_'
              << std::setw(2) << lt->tm_hour
              << std::setw(2) << lt->tm_min
              << std::setw(2) << lt->tm_sec << "] ";
}

Account::Account(int initial_deposit)
: _accountIndex(_nbAccounts), _amount(initial_deposit), _nbDeposits(0), _nbWithdrawals(0) {
    ++_nbAccounts; _totalAmount += _amount;
    _displayTimestamp();
    std::cout << "index:" << _accountIndex
              << ";amount:" << _amount << ";created\n";
}

Account::Account()
: _accountIndex(_nbAccounts), _amount(0), _nbDeposits(0), _nbWithdrawals(0) {
    ++_nbAccounts;
}

Account::~Account() {
    _displayTimestamp();
    std::cout << "index:" << _accountIndex
              << ";amount:" << _amount << ";closed\n";
}

int Account::getNbAccounts()        { return _nbAccounts; }
int Account::getTotalAmount()       { return _totalAmount; }
int Account::getNbDeposits()        { return _totalNbDeposits; }
int Account::getNbWithdrawals()     { return _totalNbWithdrawals; }

void Account::displayAccountsInfos() {
    _displayTimestamp();
    std::cout << "accounts:" << getNbAccounts()
              << ";total:" << getTotalAmount()
              << ";deposits:" << getNbDeposits()
              << ";withdrawals:" << getNbWithdrawals() << '\n';
}

void Account::makeDeposit(int deposit) {
    int prev = _amount;
    _amount += deposit; ++_nbDeposits; ++_totalNbDeposits; _totalAmount += deposit;
    _displayTimestamp();
    std::cout << "index:" << _accountIndex
              << ";p_amount:" << prev
              << ";deposit:" << deposit
              << ";amount:" << _amount
              << ";nb_deposits:" << _nbDeposits << '\n';
}

bool Account::makeWithdrawal(int withdrawal) {
    int prev = _amount;
    _displayTimestamp();
    std::cout << "index:" << _accountIndex
              << ";p_amount:" << prev
              << ";withdrawal:";
    if (withdrawal > prev) {
        std::cout << "refused\n";
        return false;
    }
    _amount -= withdrawal; ++_nbWithdrawals; ++_totalNbWithdrawals; _totalAmount -= withdrawal;
    std::cout << withdrawal
              << ";amount:" << _amount
              << ";nb_withdrawals:" << _nbWithdrawals << '\n';
    return true;
}

int Account::checkAmount() const {
    return _amount;
}

void Account::displayStatus() const {
    _displayTimestamp();
    std::cout << "index:" << _accountIndex
              << ";amount:" << _amount
              << ";deposits:" << _nbDeposits
              << ";withdrawals:" << _nbWithdrawals << '\n';
}

